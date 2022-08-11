#ifndef THREAD_H
#define THREAD_H

#include "types.h"

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;

class PCB; // kernel's implementation of a user's thread

class Thread{
public:

	void start();
	void waitToComplete(); 	// similar to join in java

	virtual ~Thread();		// Destructor of a subclass should call waitToComplete().
							// If we call it in ~Thread() our overridden run method is no longer
							// in the vector table of virtual functions.

	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID id);

	/////////////

	static ID fork();
	static void exit();
	static void waitForForkChildren();

	virtual Thread* clone() const;	// user must override it, PCB object will already be created

	////////////

protected:
	friend class PCB;

	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() { }

private:
	PCB* myPCB;
};

void dispatch();

#endif
