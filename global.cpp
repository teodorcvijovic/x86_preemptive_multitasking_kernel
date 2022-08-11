#include "global.h"

PCB* running = nullptr;
PCB* mainPCB = nullptr;
PCB* idlePCB = nullptr;

//////////////////////

bool Global::created = false;

Global::Global(){
	// wrapper is automatically placed on stack, and it is a funtion pointer that points to main

	// system stack already exists
	stackSize = 0;
	stack = nullptr;
	sp = 0;
	ss = 0;
	bp = 0;

	status = PCB::RUNNING;
	timeSlice = defaultTimeSlice;

	LOCKED(
			allPCBs.append(this);
	)
}

void Global::createGlobal(){
	if (created) return;

	mainPCB = new Global();
	running = mainPCB;

	created = true;
}

//////////////////////

bool Idle::created = false;
volatile bool Idle::spin = true;

Idle::Idle(){

	// placing the loop on idle stack
	stackSize = 256;
	initStack(spinning);

	status = PCB::IDLE;
	timeSlice = 1;
}

void Idle::createIDLE(){
	if (created) return;

	idlePCB = new Idle();

	created = true;
}

void Idle::spinning(){

	while(spin == true){

	}
}

