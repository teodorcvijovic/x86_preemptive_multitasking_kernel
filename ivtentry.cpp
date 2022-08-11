#include "ivtentry.h"
#include "kerevent.h"

#include <dos.h>

IVTEntry *IVTEntry::IVTable[IVT_SIZE] = {nullptr};

IVTEntry::IVTEntry(IVTNo _ivtNo, intPtr _newRoutine){
	ivtNo = _ivtNo;

	newRoutine = _newRoutine;
	oldRoutine = nullptr;

	event = nullptr;

	IVTable[ivtNo] = this;
}

IVTEntry::~IVTEntry(){
	restore();
}

void IVTEntry::init(KernelEv* _event){
#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED(
			oldRoutine = getvect(ivtNo);
			setvect(ivtNo, newRoutine);
	)
#endif
	event = _event;
}

void IVTEntry::restore(){
#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED(
	        setvect(ivtNo, oldRoutine);
	)
#endif
	IVTable[ivtNo] = nullptr;
	event = nullptr;
}

void IVTEntry::wakeUp(){
	event->signal();
}
