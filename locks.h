#ifndef LOCKS_H
#define LOCKS_H

#include "pcb.h"
#include "thread.h"

#include <dos.h>

extern volatile unsigned lockCount; // for nested locking
extern volatile Time countdown; 	// timer

void dispatch();

#define LOCK 				++lockCount;
#define UNLOCK 				if(--lockCount==0 && countdown==0 && !(running->unlimitedExeTime()) ) { dispatch(); }
#define LOCKED(code)		LOCK; code; UNLOCK;

// masked interrupts
#define DISABLE_INT				asm{ pushf; cli; }
#define RESTORE_INT				asm{ popf; }
#define INT_DISABLED(block)		DISABLE_INT; block; RESTORE_INT;

int syncPrintf(const char *format, ...);	// synchronized printf for testing purposes

/*
// mutual exclusion semaphore
extern int mutex;
void mutexWait();

#define intLock mutexWait();
#define intUnlock mutex = 1;
*/

/*
// mutual exclusion without interrupt disabling
extern volatile unsigned int lockFlag;
void returnLockFlag();
extern volatile bool contextSwitchOnDemand;

#define intLock lockFlag = 0;
#define intUnlock returnLockFlag();
*/

#endif
