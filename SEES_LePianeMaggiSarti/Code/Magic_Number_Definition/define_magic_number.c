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
#define EXP_MIN 482500000
#define EXP_MAX 522400000
#define EXP_INCR 10000 
#define FILE_NAME "define_magic_number_DATA.txt"


// Flags
#define T_START 1
#define T_END 2

/* Global */
struct timespec exp_start;
struct timespec exp_end;


/* Prototypes */
void startExperiments(void);
void createTimestamp(uint8_t time_flag);
void runExperiment(uint32_t exp_cycles);
void saveExperiment(int fd,uint32_t experiment_n, uint32_t exp_cycles);
double toMillis(struct timespec nanos);
void clockResolution(void);


/* MAIN */
void start(void){
	taskSpawn("Experiments", 101, 0, 4000, (FUNCPTR) startExperiments, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}


void startExperiments(void){
	uint32_t exp_cycles;	
	int fd = open(FILE_NAME, 0x202, 0777);
	uint32_t experiment_number = 0;
	sleep(1);						// Wait to clear terminal screen
	clockResolution();				// Show Clock Resolution

	for(exp_cycles=EXP_MIN; exp_cycles<=EXP_MAX; exp_cycles+=EXP_INCR){	
		/* Experiment */
		// 1 - Init
		//logMsg("-- Experiment %d --\n", experiment_number,0,0,0,0,0);		//DEBUG
		// 2 - Start Cronometer
		createTimestamp(T_START);
		// 3 - Run Experiment
		runExperiment(exp_cycles);
		// 4 - Stop Cronometer
		createTimestamp(T_END);
		// 5 - Record data
		saveExperiment(fd,experiment_number,exp_cycles);
		experiment_number++;
	}
	// 6 - Save data on file
	close(fd);
	logMsg("-- END--\n", 0,0,0,0,0,0);
}


// Busy Wait
void runExperiment(uint32_t exp_cycles){
	uint32_t i = 0;
	while(i < exp_cycles){ 
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


void saveExperiment(int fd,uint32_t experiment_n, uint32_t exp_cycles){
	char exp_log[100];
	double ms_start = toMillis(exp_start);
	double ms_end = toMillis(exp_end);
	double delta = ms_end - ms_start;
	// exp_n | computations | t_start | t_end | delta_t
	sprintf(exp_log, "%u|%u|%.2f|%.2f|%.2f\n",experiment_n, exp_cycles, ms_start, ms_end, delta);
	write(fd, exp_log, strlen(exp_log));
}


double toMillis(struct timespec nanos){
	double millis = 0;
	millis = nanos.tv_sec * 1000; 		// s * 10^3 = ms
	millis += nanos.tv_nsec / 1000000;  // ns /10^6 = ms
	return millis;	
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
