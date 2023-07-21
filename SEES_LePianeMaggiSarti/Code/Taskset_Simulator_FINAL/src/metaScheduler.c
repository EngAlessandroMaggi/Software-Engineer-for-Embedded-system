#include "metaScheduler.h"
#include "lightLogger.h"
#include "timecounter.h"
#include <taskLib.h>



/* METASCHEDULER */
void metascheduler(Meta_TCB* task_register, int64_t simulation_end_time, char* taskset_name){
	double current_time;
	int32_t index;
	int8_t status = STATUS_OK;
	double simulation_start_time = createTimestamp();
	simulation_end_time = simulation_start_time + (double)simulation_end_time;
	logNewEntry(taskIdSelf(), simulation_start_time, SIMULATION_START, 0);
	while(1){
		current_time = createTimestamp();
		/* Check if Simulation is done */
		if(current_time > simulation_end_time){
			teardownSimulation(task_register, current_time, taskset_name);
		}
		/* Check Tasks status */
		for(index=1; index<=TASK_SET_DIMENSION; index++){
			status = checkActivation(task_register[index], current_time);
			if(status == RELEASE){				
				activateTask(&task_register[index], current_time);
			}else if(status == STATUS_OK){
				status = checkDeadline(task_register[index], current_time);
				if(status == DEADLINE_MISSED){	
					logMsg("-- SIMULATION ABORTED - Deadline Missed! --\n", 0,0,0,0,0,0);
					abortSimulation(task_register, current_time, task_register[index].tsk_ID, taskset_name);
				}
			}else if(status == DEADLINE_MISSED){
				logMsg("-- SIMULATION ABORTED - Check Activation Failed! --\n", 0,0,0,0,0,0);
				abortSimulation(task_register, current_time, task_register[index].tsk_ID, taskset_name);
			}
		}
		/* Wait next activation */
		taskSuspend(0);		
	}
}



/* Check if is time to activate the task */
int8_t checkActivation(Meta_TCB tsk_TCB, double current_time){
	if(tsk_TCB.activation_period == -1) return STATUS_OK;					// Just for sporadic tasks
	if(((int64_t)current_time % (int64_t)tsk_TCB.activation_period) == 0){
		if(tsk_TCB.status == JOB_RUNNING){									// Deadline Missed
			return DEADLINE_MISSED;
		}else{
			return RELEASE;
		}
	}
	return STATUS_OK;
}


/* Activate task */
void activateTask(Meta_TCB* tsk_TCBptr, double current_time){
	tsk_TCBptr->termination_time = 0;
	tsk_TCBptr->last_activation_timestamp = current_time;
	tsk_TCBptr->status = JOB_RUNNING;
	if(tsk_TCBptr->type == SPORADIC){
		tsk_TCBptr->activation_period = getNextActivation(tsk_TCBptr, current_time);
	}
	logNewEntry(taskIdSelf(), current_time, TSK_RELEASED,tsk_TCBptr->tsk_ID);	// Logs
	taskResume(tsk_TCBptr->tsk_ID);
}


/* Get next activation time for a sporadic task */
int getNextActivation(Meta_TCB* tsk_TCBptr, double current_time){
	int next = -1;
	for(int i=0; i<SP_ACTIVATION_REGISTER_DIMENSION; i++){
		if(tsk_TCBptr->sp_activation_register[i] != 0){
			next = tsk_TCBptr->sp_activation_register[i] + (int)current_time;	// Actual time + delay
			tsk_TCBptr->sp_activation_register[i] = 0;							// Logically Eliminates the used values
			return next;
		}
	}
	return -1;
}


/* Check if task has missed its deadeline */
int8_t checkDeadline(Meta_TCB tsk_TCB, double current_time){
	if(tsk_TCB.status == JOB_TERMINATED){ return 0; }
	double absolute_deadline = tsk_TCB.last_activation_timestamp + (double)tsk_TCB.relative_deadline;
	if(current_time > absolute_deadline){	// Deadline Missed
		return DEADLINE_MISSED;
	}else{
		return STATUS_OK;
	}
}


/* SIMULATION TEARDOWN */
void teardownSimulation(Meta_TCB* task_register, double current_time, char* taskset_name){
	sysAuxClkDisable();							// Stops clock
	logNewEntry(taskIdSelf(), current_time, SIMULATION_END, 0);	// Logs end
	deleteTasks(task_register, current_time);					// Kill all task
	logDump(task_register, taskset_name);										// Save log
	logMsg("-- END --\n", 0,0,0,0,0,0);
	debugTCB(task_register);
}


void deleteTasks(Meta_TCB* task_register, double current_time){
	for(int i=1; i<=TASK_SET_DIMENSION; i++){
		taskDelete(task_register[i].tsk_ID);
	}
}


void abortSimulation(Meta_TCB* task_register, double current_time, TASK_ID tsk_error, char* taskset_name){
	sysAuxClkDisable();															// Stops clock
	deleteTasks(task_register, current_time);									// Kill all task
	debugTCB(task_register);													// Shows Meta-TCB content
	logNewEntry(taskIdSelf(), current_time, SIMULATION_ABORTED, tsk_error);		// Logs end
	logDump(task_register, taskset_name);										// Save Log
	logMsg("-- SIMULATION ABORTED --\n", 0,0,0,0,0,0);
}


/* Shows Meta-TCB content */
void debugTCB(Meta_TCB* task_register){
	for(int i=0; i<=TASK_SET_DIMENSION; i++){
		sleep(1);
		logMsg("-- TASK %d --\n", task_register[i].tsk_ID,0,0,0,0,0);
		logMsg(" Name: %s\n", task_register[i].tsk_name,0,0,0,0,0);
		logMsg(" Priority: %d\n", task_register[i].priority,0,0,0,0,0);
		logMsg(" Type: %d\n", task_register[i].type,0,0,0,0,0);
		logMsg(" Ti: %d\n", task_register[i].activation_period,0,0,0,0,0);
		logMsg(" Exec: %d\n", task_register[i].execution_time,0,0,0,0,0);
		logMsg(" Di: %d\n", task_register[i].relative_deadline,0,0,0,0,0);
		logMsg(" Activation: %ld\n", task_register[i].last_activation_timestamp,0,0,0,0,0);
		logMsg(" Termination: %ld\n", task_register[i].termination_time,0,0,0,0,0);
		logMsg(" Status: %d\n\n", task_register[i].status,0,0,0,0,0);
		if(task_register[i].type == SPORADIC){
			logMsg("Sporadic Activation Register: \n", 0,0,0,0,0,0);
			for(int i=0; i<SP_ACTIVATION_REGISTER_DIMENSION; i++){
				logMsg("\t%d: %d\n", i,task_register[i].sp_activation_register[i],0,0,0,0);
			}
			logMsg("\t---\n\n", 0,0,0,0,0,0);
		}
	}
}

