#include "kersem.h"

SemList allKernelSems;

KernelSem::KernelSem(int init){
	val = init > 0 ? init : 0;
	LOCKED(
			allKernelSems.append(this);
	)
}

KernelSem::~KernelSem(){
	LOCKED(
			unblockAllPCBs();
			allKernelSems.remove(this);
	)
}

void KernelSem::unblockAllPCBs(){
	LOCKED(
			while(!infTimeQueue.isEmpty()){
				PCB* pcb = infTimeQueue.getFirst();
				infTimeQueue.removeFirst();

				if (pcb!=nullptr) pcb->unblock();
			}
			while(!timeLimitQueue.isEmpty()){
				PCBTime* pcb_and_time = timeLimitQueue.getFirst();
				PCB* pcb = pcb_and_time->pcb;
				timeLimitQueue.removeFirst();

				if (pcb_and_time!=nullptr) { delete pcb_and_time; pcb_and_time=nullptr; }

				if (pcb!=nullptr) pcb->unblock();
			}
	)
}

int KernelSem::wait(Time maxTimeToWait){
	if(maxTimeToWait < 0) return -1;

	PCBTime* pcb_and_time = nullptr;
	int ret = 1;						// in case running thread doesn't wait on the semaphore

	LOCKED(
		if(--val < 0){
			PCB* currPCB = running;
			currPCB->signalUnblockedMe = false;

			if(maxTimeToWait == 0){
				infTimeQueue.append(currPCB);
			}
			else {
				pcb_and_time = new PCBTime(currPCB, maxTimeToWait);
				timeLimitQueue.insert(pcb_and_time);
			}

			currPCB->block();
			dispatch();

			if(pcb_and_time != nullptr) delete pcb_and_time;

			ret = currPCB->signalUnblockedMe;
			currPCB->signalUnblockedMe = false;

		}
	)
	return ret;
}

int KernelSem::signal(){
	int ret = 0;
	LOCKED(
			if (++val <= 0) ret = unblock();
	)
	return ret;
}

// else if ?
int KernelSem::unblock(){ // only one pcb can be unblocked
	int ret = 0;
	PCB* pcb = nullptr;
	LOCKED(
		if(!infTimeQueue.isEmpty()){
			pcb = infTimeQueue.getFirst();
			infTimeQueue.removeFirst();

			if(pcb != nullptr){
				pcb->unblock();
				pcb->signalUnblockedMe = true;
				ret = 1;
			}

		}
		else if(!timeLimitQueue.isEmpty()){
			pcb = timeLimitQueue.getFirst()->pcb;
			Time timeDifference = timeLimitQueue.getFirst()->waitingTime;
			timeLimitQueue.removeFirst();

			if(timeLimitQueue.getFirst() != nullptr) timeLimitQueue.getFirst()->waitingTime += timeDifference;

			if(pcb != nullptr){
				pcb->unblock();
				pcb->signalUnblockedMe = true;
				ret = 1;
			}
		}
	)
	return ret;
}

////////////////

void KernelSem::notify(){
	if(timeLimitQueue.isEmpty()) return;
	LOCKED(
			if(--(timeLimitQueue.getFirst()->waitingTime) == 0){
				while(!timeLimitQueue.isEmpty() && timeLimitQueue.getFirst()->waitingTime == 0){
					PCB* pcb = timeLimitQueue.getFirst()->pcb;
					timeLimitQueue.removeFirst();

					if(pcb != nullptr){
						pcb->unblock();
						pcb->signalUnblockedMe = false;

						++val;
					}
				}
			}
	)
}

void KernelSem::notifyAllSems(){
	for(allKernelSems.gotoFirst(); allKernelSems.hasCurr(); allKernelSems.gotoNext())
		allKernelSems.getCurr()->notify();
}
