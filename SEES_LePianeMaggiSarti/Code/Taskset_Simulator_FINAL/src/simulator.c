#include "simulator.h"
#include "metaScheduler.h"
#include "dummyTask.h"
#include "lightLogger.h"
#include <taskLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpuset.h>
#include <cpuLib.h>


/* Meta-TCB Register */
Meta_TCB task_register[TASK_SET_DIMENSION];


/* ISR */
void auxClkISR(void){
	taskResume(task_register[0].tsk_ID);
}



/* Setup and Launch Simulation */ 
void setupSimulation(void){
	cleanLog();
	/* 1 - Spawn all task */
	loadTaskRegister(task_register, TASK_SET_NAME);
	generateTaskSet(task_register);
	TASK_ID ms_id = 0;
	ms_id = taskSpawn("MetaScheduler", 99, 0, 4096, (FUNCPTR) metascheduler, task_register, SIMULATION_END_TIME, TASK_SET_NAME, 0, 0, 0, 0, 0, 0, 0);
	task_register[0].tsk_ID = ms_id;
	setCPU(ms_id);
	/* 2 - Setup Aux Clock */
	clockResolution();											// Debug
	sysAuxClkRateSet(sysClkRateGet());							// Set AuxClock rate
	/* 3 - Launch Simulation */
	sysAuxClkEnable();										   	// Enable Aux Clock
	sysAuxClkConnect((FUNCPTR) auxClkISR, (_Vx_usr_arg_t) 0);  	// Connect clock to ISR 
}



/*** UTILITIES ***/


/* Shows System Clock Resolution */
void clockResolution(void){
	int status;
	struct timespec res;
	char * array[2] = {"OK","ERROR"};
	res.tv_sec = 0;
	res.tv_nsec = 0;
	status = clock_getres (CLOCK_REALTIME, &res);
	if (status == ERROR) status = 1;
	logMsg("clock_getres status = %s\n", array[status], 0,0,0,0,0);
	logMsg("clock resolution = %d nsec = %d msec\n", (int)res.tv_nsec, res.tv_nsec/1000000, 0,0,0,0);
	sleep(1);
}



/* Loads the Task Set Infos and save them in the global register */
void loadTaskRegister(Meta_TCB* task_register, char* taskset_name){
	// TCB 0 - SCHEDULER
	strncpy(task_register[0].tsk_name, "METASCHEDULER", 100);
	task_register[0].type = 0;
	task_register[0].priority = 99;
	/* LOAD TASK SET INFOs */
	FILE *fp;
	char* filename[100];
	bzero(filename, 100);
	// Create Filename
	strncpy(filename, "taskset_", strlen("taskset_"));
	strcat(filename, taskset_name);
	strcat(filename, ".txt");
	logMsg("Filename: %s\n", filename, 0,0,0,0,0);			// Debug
	// Read File
	fp = fopen(filename, "r");
	if(fp == NULL){ printf("Error!"); exit(1); } 
	char file[5000];
	const char delim_string[2] = "|";
	const char delim_value[2] = ",";
	char* token_string;
	char* token_value;
	int tcb_index = 1;
	while(fgets(file, 5000, fp) != NULL){
		token_string = strtok(file, delim_string);
		for(int index = 0; token_string != NULL; index++){
			if(index == 0) strcpy(task_register[tcb_index].tsk_name, token_string);
			if(index == 1) task_register[tcb_index].type = atoi(token_string);
			if(index == 2) task_register[tcb_index].priority = atoi(token_string);
			if(index == 3) task_register[tcb_index].activation_period = atoi(token_string);
			if(index == 4) task_register[tcb_index].relative_deadline = atoi(token_string);
			if(index == 5) task_register[tcb_index].execution_time = atoi(token_string);
			if(index == 6) {
				token_value = strtok(token_string, delim_value);
				if(token_value == NULL){
					task_register[tcb_index].sp_activation_register[0] = 0;
				}else{
					// Get Sporadic Activations
					for(int i = 0; i < SP_ACTIVATION_REGISTER_DIMENSION && token_value != NULL; i++){
						task_register[tcb_index].sp_activation_register[i] = atoi(token_value);
						token_value = strtok(NULL, delim_value);
					}					
				}
				task_register[tcb_index].status = JOB_TERMINATED;
			}
			token_string = strtok(NULL, delim_string);
		}
		tcb_index++;
	}
}



/* Create task set  */
void generateTaskSet(Meta_TCB* task_register){
	TASK_ID tsk_id = 0;
	for(int i=1; i<=TASK_SET_DIMENSION; i++){
		tsk_id = taskSpawn(task_register[i].tsk_name, task_register[i].priority, 0, 4096, (FUNCPTR) worker, (Meta_TCB*)&task_register[i], 0, 0, 0, 0, 0, 0, 0, 0, 0);
		task_register[i].tsk_ID = tsk_id;
		setCPU(tsk_id);
	}
}



/* Set Task CPU Core of Execution to enforce Mono-Core behaviour */
STATUS setCPU(TASK_ID tsk_id){
	cpuset_t affinity;
	
	if(tsk_id == NULL)
		return(ERROR);
	
	CPUSET_ZERO(affinity);						// Reset
	CPUSET_SET(affinity,1);						// Set CPU1
	
	if(taskCpuAffinitySet(tsk_id,affinity)==ERROR){
		taskDelete(tsk_id);
		return(ERROR);
	}
	
	taskActivate(tsk_id);
	return (OK);
}
