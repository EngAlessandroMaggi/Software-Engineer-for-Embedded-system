#ifndef METASCHEDULER_H_
#define METASCHEDULER_H_
#include "simulator.h"
#include <stdint.h>

/* Task Time Status */
#define DEADLINE_MISSED -1
#define STATUS_OK 0
#define RELEASE 1

/* Task Type */
#define PERIODIC 0
#define SPORADIC 1


/* Prototypes */
void metascheduler(Meta_TCB* task_register, int64_t simulation_end_time, char* taskset_name);
int8_t checkActivation(Meta_TCB tsk_TCB, double current_time);
void activateTask(Meta_TCB* tsk_TCBptr, double current_time);
int getNextActivation(Meta_TCB* tsk_TCBptr, double current_time);
int8_t checkDeadline(Meta_TCB tsk_TCB, double current_time);
void teardownSimulation(Meta_TCB* task_register, double current_time, char* taskset_name);
void deleteTasks(Meta_TCB* task_register, double current_time);
void abortSimulation(Meta_TCB* task_register, double current_time, TASK_ID tsk_error, char* taskset_name);
void debugTCB(Meta_TCB* task_register);


#endif /* METASCHEDULER_H_ */
