#ifndef LIGHTLOGGER_H_
#define LIGHTLOGGER_H_
#include "simulator.h"

#define LOG_DIMENSION 10000

/* LOG ACTIONS */
typedef enum {
	SIMULATION_START,
	TSK_RELEASED,
	TSK_STARTJOB,
	TSK_ENDJOB,
	SIMULATION_END,
	SIMULATION_ABORTED
} action_type;



typedef struct {
	int64_t index;
	TASK_ID task_id;
	int64_t time;
	action_type action;
	int32_t value;
} LogEntry;



/* PROTOTYPES */
void cleanLog(void);
void logNewEntry(TASK_ID taskId, int64_t millis_time, action_type action, int32_t value);
void logDump(Meta_TCB* task_register, char* taskset_name);
void actionToString(action_type action, char* action_str);
void getTaskName(Meta_TCB* task_register, TASK_ID tsk_id, char* tsk_name_str);


#endif /* LIGHTLOGGER_H_ */
