#include "pcb.h"
#include "kersem.h"
#include "kerevent.h"
#include "ivtentry.h"

#include <dos.h>
#include "schedule.h"
#include <assert.h>
#include "intLock.h"

#include <iostream.h>

PCBList allPCBs;
extern SemList allKernelSems; // kersem.cpp

ID PCB::currId = 0;

///////////////////////

PCB::PCB(StackSize size, Time t, Thread* thread, Status st){
	id = ++currId;

	if (size < MIN_STACK_SIZE) size = MIN_STACK_SIZE;
	if (size > MAX_STACK_SIZE) size = MAX_STACK_SIZE;
	stackSize = size / sizeof(Data);
	initStack(PCB::wrapper);

	timeSlice = t;
	myThread = thread;
	status = st;
	myLockCount = 0;

	parent = nullptr;

	LOCKED(
			allPCBs.append(this);
	)
}

PCB::PCB(){
	id = ++currId;
	myThread = nullptr;
	parent = nullptr;
	myLockCount = 0;
}

void PCB::initStack(funPtr f){
	LOCKED(
			stack = new Data[stackSize];
	)
			stack[stackSize - 1] = PSW_START_VALUE;
#ifndef BCC_BLOCK_IGNORE
			stack[stackSize - 2] = FP_SEG(f);
			stack[stackSize - 3] = FP_OFF(f);

			// from stack[stackSize - 4] to stack[stackSize - 11] reserved for regs that are places on stack when entering an interrupt routine

			stack[stackSize - 12] = 0;				// BP = 0, so we know we need to stop changing childs BPs in fork

			ss = FP_SEG(stack + stackSize - 12); 	// stack points to last taken location
			sp = FP_OFF(stack + stackSize - 12);
			bp = sp;
#endif
}


void PCB::wrapper(){
	running->getMyThread()->run();
	LOCKED(
			running->setStatus(TERMINATED);
			running->notifySusupendedPCBs();
			running->notifyParent();
			dispatch();
	)
}

void PCB::notifySusupendedPCBs(){
	if(this == nullptr) return;
	LOCKED(
			for(waitingForMe.gotoFirst(); waitingForMe.hasCurr(); waitingForMe.removeFirst()){
				PCB* pcb = waitingForMe.getFirst();
				if(pcb!=nullptr) pcb->unblock();
			})
}

////////////////////////

// if we call waitToComplete for a suspended thread, maybe another thread will make the suspended thread runnable again

void PCB::waitToComplete(){ // if the thread is not started, function won't have any effect
	if(this == nullptr) return;
	LOCKED(
			if(running != this && status != TERMINATED && status != IDLE && status != INITIALIZED){
				waitingForMe.append(running);
				running->block();
				dispatch();
			}
	)
}


void PCB::start(){ // can be called once per object
	LOCKED(
		if(status == INITIALIZED){
			setStatus(READY);
			Scheduler::put(this);
		}
	)
}

void PCB::block(){
	LOCKED(
		if(status!=TERMINATED && status!= IDLE) setStatus(SUSPENDED);
	)
}

void PCB::unblock(){
	LOCKED(
			if(status == SUSPENDED){
				setStatus(READY);
				Scheduler::put(this);
			}
		)
}

//////////////////

void PCB::removeMeFromWaitingQueues(){
	// remove this pcb from other pcbs' waiting queues
	for(allPCBs.gotoFirst(); allPCBs.hasCurr(); allPCBs.gotoNext()){
		PCB* pcb = allPCBs.getCurr();
		if(pcb){
			for(pcb->waitingForMe.gotoFirst(); pcb->waitingForMe.hasCurr(); ){
				PCB* p = pcb->waitingForMe.getCurr();
				pcb->waitingForMe.gotoNext();				// with this, we ensure that we won't delete curr
				if(p == this) pcb->waitingForMe.remove(p);
			}
		}
	}

	// remove this pcb from other kernel semaphores' waiting queues
	for(allKernelSems.gotoFirst(); allKernelSems.hasCurr(); allKernelSems.gotoNext() ){
		KernelSem* sem = allKernelSems.getCurr();
		if(sem){
			for(sem->infTimeQueue.gotoFirst(); sem->infTimeQueue.hasCurr(); ){
				PCB* p = sem->infTimeQueue.getCurr();
				sem->infTimeQueue.gotoNext();
				if(p == this) sem->infTimeQueue.remove(p);
			}
			for(sem->timeLimitQueue.gotoFirst(); sem->timeLimitQueue.hasCurr(); ){
				PCBTime* ptime = sem->timeLimitQueue.getCurr();
				sem->timeLimitQueue.gotoNext();
				if(ptime->pcb == this) {
					sem->timeLimitQueue.remove(ptime);
					delete ptime; 							// deleting the helper struct
				}
			}
		}
	}


	// if the pcb is wainting for an event, remove that pcb from the event
	for (int i = 0; i < IVT_SIZE; i++) {
	        KernelEv* event = IVTEntry::IVTable[i]->event;
	        if (event != nullptr) {
	        	if(event->blockedPCB == this) event->blockedPCB = nullptr;
	        	//event->blockedPCBs.remove(this);
	        }
	}

}

void PCB::freeStack(){
	if(this == nullptr) return;
	if(stack != nullptr) delete[] stack;
	stack = nullptr;
}

PCB::~PCB(){
	LOCKED(
			freeStack();
			allPCBs.remove(this);
	)
}

////////////////

PCB* PCB::getPCB(ID id){
	for(allPCBs.gotoFirst(); allPCBs.hasCurr(); allPCBs.gotoNext()){
			PCB* pcb = allPCBs.getCurr();
			if (pcb->id == id) return pcb;
	}
	return nullptr; // ?
}

////////////////

#include <stdio.h>

PCB* parentPCB;
Thread* childThr;

Reg sst;
Reg spt;
Reg bpt;
Reg oldBp;

Data* bpPtr;
Data* nextBpPtr;
Data* stackPtr;
Data* newBpPtr;
Data* nextNewBpPtr;
Data* newStackPtr;

unsigned offset;
StackSize i;

void interrupt PCB::initChildStack(...){

#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED(
		memcpy(childThr->myPCB->stack, parentPCB->stack, parentPCB->stackSize * sizeof(Data) );
	)
#endif

	////////////// Recalculate BPs /////////////

	asm{
		mov sst, ss
		mov spt, sp
		mov bpt, bp
	}

	bpPtr = (Data*) MK_FP(sst,bpt);
	newBpPtr = bpPtr - parentPCB->stack + childThr->myPCB->stack;

	stackPtr = (Data*) MK_FP(sst, spt);
	newStackPtr = stackPtr - parentPCB->stack + childThr->myPCB->stack;

	childThr->myPCB->stackSize = parentPCB->stackSize;
	childThr->myPCB->ss = FP_SEG(newStackPtr);
	childThr->myPCB->sp = FP_OFF(newStackPtr);
	childThr->myPCB->bp = FP_OFF(newBpPtr);

	/*
	printf("stackSize P = %4u\n", parentPCB->stackSize);
	printf("stackSize C = %4u\n", parentPCB->stackSize);
	printf("   sst, bpt = %5u : %5u\n", sst, bpt);
	printf("      bpPtr = %5u : %5u\n", FP_SEG(bpPtr), FP_OFF(bpPtr));
	printf("   newBpPtr = %5u : %5u\n", FP_SEG(newBpPtr), FP_OFF(newBpPtr));
	printf("   sst, spt = %5u : %5u\n", sst, spt);
	printf("   stackPtr = %5u : %5u\n", FP_SEG(stackPtr), FP_OFF(stackPtr));
	printf("newStackPtr = %5u : %5u\n", FP_SEG(newStackPtr), FP_OFF(newStackPtr));
	*/

	//for(i = 0; i < parentPCB->stackSize; i++) printf("%5u : %5u = %5u | %5u : %5u = %5u\n", FP_SEG(&parentPCB->stack[i]), FP_OFF(&parentPCB->stack[i]), parentPCB->stack[i], FP_SEG(&childThr->myPCB->stack[i]), FP_OFF(&childThr->myPCB->stack[i]), childThr->myPCB->stack[i]);
	//printf("change bps on childs stack - begin\n");

	while( (*newBpPtr) != 0 ){

		bpt = *newBpPtr;
		nextBpPtr = (Data*) MK_FP(sst, bpt);
		nextNewBpPtr = nextBpPtr - bpPtr + newBpPtr;
		*newBpPtr = FP_OFF(nextNewBpPtr);
		newBpPtr = nextNewBpPtr;
		bpPtr = nextBpPtr;

		//printf("newBpPtr = %5u : %5u\n", FP_SEG(newBpPtr), FP_OFF(newBpPtr));
		//printf("*newBpPtr = %5u\n", *newBpPtr);
	}

	//printf("change bps on childs stack - end\n");
	//for(i = 0; i < parentPCB->stackSize; i++) printf("%5u : %5u = %5u | %5u : %5u = %5u\n", FP_SEG(&parentPCB->stack[i]), FP_OFF(&parentPCB->stack[i]), parentPCB->stack[i], FP_SEG(&childThr->myPCB->stack[i]), FP_OFF(&childThr->myPCB->stack[i]), childThr->myPCB->stack[i]);

	///////////// Start the Child ///////////////

	childThr->myPCB->start();

}

ID PCB::forkWrapper(){

	parentPCB = running;
	childThr = parentPCB->myThread->clone();

	ID pId = parentPCB->id;
	ID chId = childThr->myPCB->id;

	if (childThr == nullptr || parentPCB->stack == nullptr || childThr->myPCB->stackSize < parentPCB->stackSize) return -1;

	childThr->myPCB->parent = parentPCB;
	childThr->myPCB->myLockCount = parentPCB->myLockCount;
	childThr->myPCB->timeSlice = parentPCB->timeSlice;

	(parentPCB->myChildren).append(childThr->myPCB);

	PCB::initChildStack();

	if (running->id == pId ){ return childThr->myPCB->id; }
	else if(running->id == chId) { return 0; }

	// fork failed

	childThr->myPCB->setStatus(PCB::SUSPENDED);
	(parentPCB->myChildren).remove(childThr->myPCB);

	return -1;
}

//////////////

void PCB::detachFromChildren(){
	for(myChildren.gotoFirst(); myChildren.hasCurr(); myChildren.removeFirst()){
			PCB* child = myChildren.getCurr();
			if (child!=nullptr) child->parent = nullptr;
	}
}

//////////////

void PCB::waitForForkChildren(){
	if(running->myChildren.isEmpty()) return;
	running->block();
	dispatch();
}

void PCB::notifyParent(){
	LOCKED(
		if(parent!=nullptr){
			parent->myChildren.remove(this);

			if(parent->myChildren.isEmpty()){
				parent->unblock();
			}
		}
	)
}

