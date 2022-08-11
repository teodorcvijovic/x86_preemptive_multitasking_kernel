//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    bounded.cpp
//   Date:    Maj 2021
//===========================================================================//
#include <stdlib.h>
#include "bounded.h"
#include "intLock.h"

BoundedBuffer::BoundedBuffer (unsigned size) : Size(size),
	mutexa(1), mutext(1), spaceAvailable(size), itemAvailable(0),	// mutex value is initially 1, because only one producer can append in one moment
	head(0), tail(0) {
		buffer = new char[size];
		if (!buffer) exit(1);
	}
	
BoundedBuffer::~BoundedBuffer(){
	intLock
	delete [] buffer;
	intUnlock
}

int BoundedBuffer::append (char d) {
	spaceAvailable.wait(0);
	mutexa.wait(0);						// multiple producers cannot append at the same time
		buffer[tail] = d;				// one producer will pass by the semaphore, while others will have to wait for current producer to finish
		tail = (tail+1)%Size;
	mutexa.signal();					// when producer is finished, he will singalize the semaphore mutexa so next producer in line can pass
	itemAvailable.signal();
	return 0;
}

char BoundedBuffer::take () {
	itemAvailable.wait(0);
	mutext.wait(0);						// multiple producers cannot take at the same time
		char d = buffer[head];
		head = (head+1)%Size;
	mutext.signal();
	spaceAvailable.signal();
	return d;
}
