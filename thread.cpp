#include "thread.h"

#include "pcb.h"
#include "locks.h"

#include "timer.h"
#include "types.h"

#include "list_pcb.h"

extern volatile bool contextSwitchOnDemand; 	// defined in timer.cpp

Thread::Thread (StackSize stackSize, Time timeSlice){
	LOCKED(
			myPCB = new PCB(stackSize, timeSlice, this);
	)
}

Thread::~Thread(){
	// waitToComplete should be called in subclass destructor
	LOCKED(
		if(myPCB!=nullptr){
			myPCB->removeMeFromWaitingQueues();
			myPCB->detachFromChildren();
			myPCB->notifyParent();

			delete myPCB;

		}
		myPCB = nullptr;
	)
}

///////////////////

void Thread::start(){
    if(myPCB != nullptr){
        LOCKED(myPCB->start();)
    }
}

void Thread::waitToComplete() {
    if(myPCB != nullptr){
        LOCKED(myPCB->waitToComplete();)
    }
}

ID Thread::getId(){
    if(myPCB != nullptr) return myPCB->getId();
    else return -1;
}

ID Thread::getRunningId(){
    if(running != nullptr) return running->getId();
    else return -1;
}

Thread* Thread::getThreadById(ID id){
	return PCB::getPCB(id)->getMyThread();
}

/////////////////////

void dispatch(){
#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED(
        contextSwitchOnDemand = true;
        asm int timerEntry
	)
#endif
}

/////////////////////

ID Thread::fork(){
	ID ret = -1;

	LOCKED(
			if(running != nullptr) ret = PCB::forkWrapper();
	)

	return ret;
}

void Thread::exit(){	// similar to wrapper ending
	LOCKED(
			running->setStatus(PCB::TERMINATED);
			running->notifySusupendedPCBs();
			running->notifyParent();
			dispatch();
	)
}

void Thread::waitForForkChildren(){
	LOCKED(
			PCB::waitForForkChildren();
	)
}

Thread* Thread::clone() const{
	// user must either call default Thread constructor in overriden method, or it will be implicitly called
	return nullptr;
}


