#include "timecounter.h"
#include <time.h>

/* Create a timestamp - return millis */
double createTimestamp(void){
	struct timespec current_time;
	if(clock_gettime(CLOCK_MONOTONIC, &current_time) == -1){
		logMsg("FAIL: failed to get time from clock - START\n", 0,0,0,0,0,0);
		return 0;
	}else{
		double millis = 0;
		millis = current_time.tv_sec * 1000;		 // s * 10^3 = ms
		millis += current_time.tv_nsec / 1000000;  	 // ns /10^6 = ms
		return millis;
	}
}


