#include "lightLogger.h"
#include <ioLib.h>
#include <string.h>

/* Globals */
LogEntry logRegister[LOG_DIMENSION];
static int64_t log_index = 1;


// CLEAN LOG
void cleanLog(void){
	for(int64_t i=0; i<LOG_DIMENSION; i++){
		logRegister[i].index = 0;
		logRegister[i].task_id = 0;
		logRegister[i].time = 0;
		logRegister[i].action = 0;
		logRegister[i].value = 0;
	}
}


// LOG NEW ENTRY
void logNewEntry(TASK_ID taskId, int64_t millis_time, action_type action, int32_t value){
	// Register new data
	logRegister[log_index].index = log_index;
	logRegister[log_index].task_id = taskId;
	logRegister[log_index].time = millis_time;
	logRegister[log_index].action = action;
	logRegister[log_index].value = value;
	// Increment index
	log_index++;
}


// LOG DUMP
void logDump(Meta_TCB* task_register, char* taskset_name){
	char* filename[100];
	bzero(filename, 100);
	strncpy(filename, "log_", strlen("log_"));
	strcat(filename, taskset_name);
	strcat(filename, ".txt");
	int fd = open(filename, 0x202, 0777);
	if(fd == ERROR){									// Debug
		logMsg("ERROR SAVING FILE!\n", 0,0,0,0,0,0);		
		char tmpPath[500];
		getwd(tmpPath);
		logMsg("PATH: %s\n", tmpPath,0,0,0,0,0);			
	}
	/* Read log and writes on file */
	char l_entry[100];
	char tsk_nm[TSK_NAME_MAX_SIZE];
	char action_str[100];
	for(int i=1; i<LOG_DIMENSION; i++){
		if(logRegister[i].index != 0){
			bzero(l_entry, 500);					// Init
			bzero(tsk_nm, TSK_NAME_MAX_SIZE);		// Init
			bzero(action_str, 100);					// Init
			getTaskName(task_register,logRegister[i].task_id,tsk_nm);
			actionToString(logRegister[i].action, action_str);
			// entry_n | millis | tsk_ID | tsk_name | action | value
			if(logRegister[i].value > 0){
				sprintf(l_entry, "%d|%ld|%d|%s|%s|%d\n", logRegister[i].index, logRegister[i].time, logRegister[i].task_id, tsk_nm, action_str, logRegister[i].value);
			}else{
				sprintf(l_entry, "%d|%ld|%d|%s|%s\n", logRegister[i].index, logRegister[i].time, logRegister[i].task_id, tsk_nm, action_str);
			}
			write(fd, l_entry, strlen(l_entry));			
		}
	}
	close(fd);
}


/* Utilities */

void actionToString(action_type action, char* action_str){
	int action_max_size = 50;
	switch(action){
		case 0: strncpy(action_str, "SIMULATION_START", action_max_size); break;
		case 1: strncpy(action_str, "TSK_RELEASED", action_max_size); break;
		case 2: strncpy(action_str, "TSK_STARTJOB", action_max_size); break;
		case 3: strncpy(action_str, "TSK_ENDJOB", action_max_size); break;
		case 4: strncpy(action_str, "SIMULATION_END", action_max_size); break;
		case 5: strncpy(action_str, "SIMULATION_ABORTED", action_max_size); break;
		default: strncpy(action_str, "ERROR", action_max_size);
	}
}


void getTaskName(Meta_TCB* task_register, TASK_ID tsk_id, char* tsk_name_str){
	for(int i=0; i<=TASK_SET_DIMENSION; i++){
		if(task_register[i].tsk_ID == tsk_id){
			strncpy(tsk_name_str, task_register[i].tsk_name, TSK_NAME_MAX_SIZE);
			return;
		}
	}
	strncpy(tsk_name_str, "UNKNOWN", TSK_NAME_MAX_SIZE);
}
