#ifndef PCB_H
#define PCB_H

#include "list_pcb.h"
#include "types.h"
#include "locks.h"
#include "thread.h"

extern PCB* running;
extern PCB* mainPCB;
extern PCB* idlePCB;
extern PCBList allPCBs;

class PCB{
public:

	enum Status{ INITIALIZED, READY, RUNNING, SUSPENDED, TERMINATED, IDLE };

	PCB(StackSize size, Time t, Thread* thread, Status st = INITIALIZED);
	virtual ~PCB();

	static PCB* getPCB(ID id);

	Thread* getMyThread() const { return myThread; }
	StackSize getStackSize() const { return stackSize; }
	ID getId() const { return id; }
	Status getStatus() const { return status; }
	Time getTimeSlice() const { return timeSlice; }

	bool unlimitedExeTime() const { return timeSlice == 0; }

	void setStatus(Status s) { status = s; }
	void start();
	void block();
	void unblock();

	void waitToComplete();
	void notifySusupendedPCBs();			// method is called after run()

	////////////////

	static void interrupt initChildStack(...);

	static ID forkWrapper();

	////////////////

	static void waitForForkChildren();

	void notifyParent(); 					// called when child is finished

	///////////////

protected:
	friend class Thread;
	friend class Timer;
	friend class Global;
	friend class KernelSem;
	friend class KernelEv;

	//// fields ////

	StackSize stackSize;
	Data* stack;
	volatile Reg ss, sp, bp;

	Time timeSlice;

	Thread *myThread;
	PCBList waitingForMe; 		// pcb that called waitToComplete() is inserted in this list

	Status status;
	unsigned myLockCount;

	ID id;
	static ID currId;

	bool signalUnblockedMe; 	// flag used in KernelSem

	////////////////

	PCB* parent;				// nullptr before fork()

	PCBList myChildren;			// empty before fork()

	void detachFromChildren();	// sets children's parent pointer to nullptr and deletes myChildren list
								// children are not deleted

	////////////////

	void initStack(funPtr f); 	// f points to run() method
	static void wrapper(); 		// running thread run() method execution

	PCB(); 						// base constructor for derived Global and Idle

private:
	void removeMeFromWaitingQueues(); 	// in other pcbs, semaphores and events
										// called from thread destructor
	void freeStack();
};


#endif
