#include "semaphor.h"
#include "kersem.h"

Semaphore::Semaphore(int init){
    LOCKED(
        myImpl = new KernelSem(init);
    )
}

Semaphore::~Semaphore(){
    LOCKED(
        if(myImpl != nullptr){
            delete myImpl;
            myImpl = nullptr;
        }
    )
}

int Semaphore::wait(Time maxTimeToWait){
    int ret = myImpl->wait(maxTimeToWait);
    return ret;
}

int Semaphore::signal() {
    int ret = myImpl->signal();
    return ret;
}

int Semaphore::val() const {
    int ret = myImpl->getVal();
    return ret;
}
