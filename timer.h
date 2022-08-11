#ifndef TIMER_H
#define TIMER_H

#include "locks.h"
#include "pcb.h"
#include <iostream.h>
#include <dos.h>

#include "global.h"
#include "SCHEDULE.H"

extern volatile bool contextSwitchOnDemand;
extern volatile Time countdown;
//extern volatile unsigned int lockFlag;

extern PCB* running;
extern PCB* mainPCB;
extern PCB* idlePCB;

class Timer{
public:
	static void interrupt timer(...);
	static intPtr oldTimer;

	static void init();
	static void restore();
};

#endif
