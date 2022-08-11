#ifndef KEREVENT_H
#define KEREVENT_H

#include "ivtentry.h"
#include "event.h"
#include "pcb.h"

#include "types.h"
#include "locks.h"

class KernelEv{
public:
    KernelEv(IVTNo _ivtNo);
    ~KernelEv();

    void wait();
    void signal();

    //void unblockPCBs(); // called from destructor and PCB::removeMeFromWaitingQueues

    friend class IVTEntry;
    friend class PCB;

private:
    IVTNo ivtNo;
    PCB* creatorPCB;
    PCB* blockedPCB; // only one pcb can wait on the event
    //PCBList blockedPCBs;

    bool val;
    //int val;
};

#endif
