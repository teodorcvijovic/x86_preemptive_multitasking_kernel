#ifndef GLOBAL_H
#define GLOBAL_H

#include "pcb.h"
#include "timer.h"

// global lists for PCBs and Semaphores are defined in pcb.cpp and kersem.cpp files
// global pointers are defined in pcb.cpp

extern PCB* running;
extern PCB* mainPCB;
extern PCB* idlePCB;
extern PCBList allPCBs;

///////// mainPCB ///////////

class Global : public PCB {
public:
	static void createGlobal();

private:
	friend class PCB;

	Global();
	static bool created;
};

///////// idlePCB ///////////

// idle process for freezing prevention

class Idle : public PCB {
public:
	static void createIDLE();

private:
	friend class PCB;

	Idle();
	static bool created;

	static volatile bool spin;
	static void spinning();
};


#endif
