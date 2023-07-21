#ifndef DUMMYTASK_H_
#define DUMMYTASK_H_
#include "simulator.h"

/* Define number of computations to occupy achieve a 1sec computation time */
#define MAGIC_NUMBER 500900000


/* prototypes */
void worker(Meta_TCB* tsk_TCBptr);
void get_busy(int32_t seconds);


#endif /* DUMMYTASK_H_ */
