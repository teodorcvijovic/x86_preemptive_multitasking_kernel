#include "event.h"
#include "kerevent.h"

#include "types.h"
#include "locks.h"

Event::Event(IVTNo ivtNo) {
    LOCKED(
        myImpl = new KernelEv(ivtNo);
    )
}

Event::~Event() {
    LOCKED(
        delete myImpl;
        myImpl = nullptr;
    )
}

void Event::wait() {
    myImpl->wait();
}

void Event::signal() {
    myImpl->signal();
}
