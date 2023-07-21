/* includes */
#include <vxWorks.h>
#include <sysLib.h>
#include <taskLib.h>
#include <time.h>
#include <clockLib.h>
#include <string.h>
#include <stdint.h>		
#include <limits.h>		


/* EXPERIMENT SETTINGS */
#define MAGIC_NUMBER 500900000
#define SET_CYCLES 10
#define FILE_NAME "test_MN_holding_set_DATA.txt"


// Flags
#define T_START 1
#define T_END 2

/* Global */
struct timespec exp_start;
struct timespec exp_end;


/* Prototypes */
void startExperiments(void);
void compute(long cycles);
void createTimestamp(uint8_t time_flag);
int32_t saveExperiment(int fd,uint32_t experiment_n, long computations, int32_t target_time);
double toMillis(struct timespec nanos);
void clockResolution(void);
uint32_t computeDelta(void);



/* MAIN */
void start(void){
	taskSpawn("Experiments_set", 101, 0, 4000, (FUNCPTR) startExperiments, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}


void startExperiments(void){
	int fd = open(FILE_NAME, 0x202, 0777);
	uint32_t experiment_number = 0;
	sleep(1);						// Wait to clear terminal screen
	clockResolution();				// Show Clock Resolution

	for(int32_t set_cycle=0; set_cycle<SET_CYCLES; set_cycle++){
		for(int32_t target_time = 2000; target_time<=10000; target_time+=1000){				// Experiments Set
			logMsg("-- Experiment %d --\n", experiment_number,0,0,0,0,0);					// Debug
			double multiplier = target_time/1000;
			long computations = (long)MAGIC_NUMBER * (long)multiplier;
			/* Compute */
			createTimestamp(T_START); 			// Start Cronometer
			compute(computations);				// Run Experiment
			createTimestamp(T_END); 			// Stop Cronometer
			saveExperiment(fd,experiment_number,computations, target_time);
			experiment_number++;
		}
	}

	// Save
	close(fd);
	logMsg("-- END--\n", 0,0,0,0,0,0);
}


// Busy Wait
void compute(long cycles){
	long i = 0;
	while(i < cycles){ 
		i++;
	}
}


void createTimestamp(uint8_t time_flag){
	struct timespec current_time;
	if(clock_gettime(CLOCK_MONOTONIC, &current_time) == -1){ 					
		logMsg("FAIL: failed to get time from clock - START\n", 0,0,0,0,0,0);
	}else{
		if( time_flag == T_START){
			exp_start.tv_sec = current_time.tv_sec;
			exp_start.tv_nsec = current_time.tv_nsec;
		}else{
			exp_end.tv_sec = current_time.tv_sec;
			exp_end.tv_nsec = current_time.tv_nsec;
		}
	}
}


int32_t saveExperiment(int fd,uint32_t experiment_n, long computations, int32_t target_time){
	char exp_log[500];
	double ms_start = toMillis(exp_start);
	double ms_end = toMillis(exp_end);
	double delta = ms_end - ms_start;
	// exp_n | target_t | computations | ms_start | ms_end | delta
	sprintf(exp_log, "%u|%d|%ld|%.2f|%.2f|%.2f\n",experiment_n, target_time, computations, ms_start, ms_end, delta);
	write(fd, exp_log, strlen(exp_log));
	int int_delta = (int)delta;
	return (int32_t)int_delta;
}


double toMillis(struct timespec nanos){
	double millis = 0;
	millis = nanos.tv_sec * 1000; 		// s * 10^3 = ms
	millis += nanos.tv_nsec / 1000000;  // ns /10^6 = ms
	return millis;	
}


uint32_t computeDelta(void){
	double ms_start = toMillis(exp_start);
	double ms_end = toMillis(exp_end);
	double delta = ms_end - ms_start;
	int int_delta = (int)delta;
	return (uint32_t)int_delta;
}


void clockResolution(void){
	int status;
	struct timespec res;
	char * array[2] = {"OK","ERROR"};
	res.tv_sec = 0;
	res.tv_nsec = 0;
	status = clock_getres (CLOCK_REALTIME, &res);
	if (status == ERROR) status = 1;
	printf("clock_getres status = %s\n", array[status]);
	printf("clock resolution = %d nsec = %f msec\n", (int)res.tv_nsec, (float)(float)res.tv_nsec/1000000);
	sleep(1);
}




