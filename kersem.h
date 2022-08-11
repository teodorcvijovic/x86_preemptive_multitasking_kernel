#ifndef KERSEM_H
#define KERSEM_H

#include "types.h"
#include "locks.h"

#include "list_pcb.h"
#include "list_sem.h"
#include "sem_help.h"

extern SemList allKernelSems;

class KernelSem{
public:

	KernelSem(int init);
	~KernelSem();

	int wait(Time maxTimeToWait);
	int signal();
	int getVal() const { return val; }

	friend class PCB;
	friend class Timer;

protected:
	void unblockAllPCBs();			// called from the destructor
	int unblock(); 					// called in signal method

	void notify(); 					// helper for notifyAllSems
	static void notifyAllSems();	// called in timer every 55ms

	/////// fields ///////

	int val;
	PCBList infTimeQueue;
	PCBTimeList timeLimitQueue;

};

#endif
