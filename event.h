#ifndef EVENT_H
#define EVENT_H

typedef unsigned char IVTNo;
class KernelEv;

class Event {
public:
    Event (IVTNo ivtNo);
    ~Event ();

    void wait ();

protected:
    friend class KernelEv;
    void signal(); 				// can call KernelEv

private:
    KernelEv* myImpl;
};

#endif
