#ifndef SIMULATOR_H_
#define SIMULATOR_H_
#include <taskLib.h>

/* Taskset Properties */
#define TASK_SET_NAME "TS_SPORADIC_TEST"
#define SIMULATION_END_TIME 100000
#define TASK_SET_DIMENSION 2
#define SP_ACTIVATION_REGISTER_DIMENSION 5

/* Meta-TCB stuff*/
#define TSK_NAME_MAX_SIZE 100
#define JOB_RUNNING 1
#define JOB_TERMINATED 2


typedef struct {
	TASK_ID tsk_ID;
	char tsk_name[TSK_NAME_MAX_SIZE];
	int priority;
	int type;					// Periodic or Sporadic
	int activation_period;
	int execution_time;
	int relative_deadline;
	int sp_activation_register[SP_ACTIVATION_REGISTER_DIMENSION];	// Sporadic activation times
	double last_activation_timestamp;
	double termination_time;
	int status;					// JOB status
} Meta_TCB;



/* Prototypes */
void auxClkISR(void);
void setupSimulation(void);
void clockResolution(void);
void loadTaskRegister(Meta_TCB* task_register, char* taskset_name);
void generateTaskSet(Meta_TCB* task_register);
STATUS setCPU(TASK_ID tsk_id);


#endif /* SIMULATOR_H_ */
