#ifndef SEMAPHOR_H
#define SEMAPHOR_H

#include "types.h"

class KernelSem;

class Semaphore {
public:
    Semaphore (int init=1);
    virtual ~Semaphore ();

    virtual int wait (Time maxTimeToWait);
    virtual int signal();

    int val () const; // Returns the current value of the semaphore

private:
     KernelSem* myImpl;
};




#endif
