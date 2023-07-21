#include <taskLib.h>
#include "dummyTask.h"
#include "lightLogger.h"
#include "timecounter.h"


/* Dummy Task */ 
void worker(Meta_TCB* tsk_TCBptr){
	double my_timestamp;
	while(1){
		taskSuspend(0);
		my_timestamp = createTimestamp();		
		logNewEntry(taskIdSelf(),  my_timestamp, TSK_STARTJOB, 0);							// Log Start Job
		logMsg("-->%s: START JOB at %d\n", tsk_TCBptr->tsk_name,my_timestamp,0,0,0,0);		// Debug
		get_busy(tsk_TCBptr->execution_time);												// Waste time
		my_timestamp = createTimestamp();
		logNewEntry(taskIdSelf(), my_timestamp, TSK_ENDJOB, 0);								// Log End Job
		tsk_TCBptr->termination_time = my_timestamp;
		tsk_TCBptr->status = JOB_TERMINATED;
		logMsg("-->%s: END JOB at %d\n", tsk_TCBptr->tsk_name,my_timestamp,0,0,0,0);		// Debug
	}
}


/* Busy Wait */
void get_busy(int32_t seconds){
	int64_t cycles = (int64_t)MAGIC_NUMBER * (int64_t)seconds;
	int64_t i = 0;
	while(i < cycles){ 
		i++;
	}
}
