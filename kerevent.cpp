#include "kerevent.h"
#include "ivtentry.h"

// before Event object is created, macro PREPAREENTRY must be used to initialize the entry in interrupt vector table

KernelEv::KernelEv(IVTNo _ivtNo){
	ivtNo = _ivtNo;
	creatorPCB = running;
	blockedPCB = nullptr;

	val = false;

	IVTEntry::IVTable[ivtNo]->init(this); 	// IVTEntry object is prepared and created before Event constructor is called
}

KernelEv::~KernelEv(){
	LOCKED(
		if(blockedPCB!=nullptr){
			blockedPCB->unblock();
			blockedPCB = nullptr;
		}

		IVTEntry::IVTable[ivtNo]->restore();
		creatorPCB = nullptr;
	)
}

void KernelEv::wait(){
	if(running != creatorPCB) return;

	LOCKED(
		if(val == false){
			if(blockedPCB == nullptr){
				blockedPCB = running;
				blockedPCB->block();
				dispatch();
			}
		}
		else val = false;
	)

}

void KernelEv::signal(){
	LOCKED(
			if(blockedPCB != nullptr) {
				blockedPCB->unblock();
				blockedPCB = nullptr;
			}
			else val = true;
	)
}

/////////////////////

/*
void KernelEv::unblockPCBs(){
	LOCKED(
	if(!blockedPCBs.isEmpty()){
			for(blockedPCBs.gotoFirst(); blockedPCBs.hasCurr(); blockedPCBs.removeFirst()){
			PCB* pcb = blockedPCBs.getFirst();
			if (pcb != nullptr) pcb->unblock();
		}
	}
	)
}

KernelEv::KernelEv(IVTNo _ivtNo){
	ivtNo = _ivtNo;
	creatorPCB = running;

	val = 0;

	IVTEntry::IVTable[ivtNo]->init(this); 	// IVTEntry object is prepared and created before Event constructor is called
}

KernelEv::~KernelEv(){
	LOCKED(
		unblockPCBs();
		IVTEntry::IVTable[ivtNo]->restore();
		creatorPCB = nullptr;
	)
}

void KernelEv::wait(){
	if(running != creatorPCB) return;
	LOCKED(
		if(--val < 0){
			PCB* pcb = running;
			pcb->block();
			blockedPCBs.append(pcb);
			dispatch();
		}
	)
}

void KernelEv::signal(){
	LOCKED(
		if(val++ < 0){
			PCB* pcb = blockedPCBs.getFirst();
			blockedPCBs.removeFirst();
			if (pcb != nullptr) pcb->unblock();
		}
		else val = 1;
	)
}

*/


