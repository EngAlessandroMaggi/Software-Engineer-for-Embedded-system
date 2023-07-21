/* includes */
#include "src/simulator.h"
#include <vxWorks.h>
#include <taskLib.h>

/* MAIN */
void start(){
	logMsg("Simulation Started!\n", 0,0,0,0,0,0);													// Debug
	taskSpawn("Launcher", 98, 0, 4000, (FUNCPTR) setupSimulation, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
