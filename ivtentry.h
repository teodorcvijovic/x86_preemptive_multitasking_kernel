#ifndef IVTENTRY_H
#define IVTENTRY_H

#include "event.h"
#include "kerevent.h"
#include "types.h"
#include "locks.h"

// only one event can wait on an ivt entry

class IVTEntry {
public:

	IVTEntry(IVTNo _ivtNo, intPtr _newRoutine);
	~IVTEntry();

	void init(KernelEv* _event); 				// set event and change routines
	void restore();								// restore the old routine

	void wakeUp();

	/////// interrupt vector table ///////

	static IVTEntry *IVTable[IVT_SIZE]; 		// pointer table

	/////////////// fields ///////////////

	intPtr oldRoutine;
	intPtr newRoutine;

	IVTNo ivtNo;

	KernelEv* event;
};

/////// construction of IVTEntry object and definition of an interrupt routine ///////

#define PREPAREENTRY(ivtNo,callOldRoutine)          			\
void interrupt interruptRoutine##ivtNo(...);        			\
                                                    			\
IVTEntry ivtEntry##ivtNo(ivtNo, interruptRoutine##ivtNo);       \
void interrupt interruptRoutine##ivtNo(...){     				\
	if(callOldRoutine) ivtEntry##ivtNo.oldRoutine();          	\
    ivtEntry##ivtNo.wakeUp();                       			\
    dispatch();													\
}

////////////////////////////////////////////////////////////////////////////////////////

#endif
