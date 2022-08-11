#include "locks.h"


/*
// lockFlag
volatile unsigned int lockFlag = 1;

void returnLockFlag(){
	lockFlag = 1;
	if(contextSwitchOnDemand){ dispatch(); }
}
*/

/*
// mutex

int mutex = 1;

void mutexWait() {
	tryagain:asm{
		mov ax, 0
		xchg ax, mutex
	}
	if(_AX ==0){
		dispatch(); 		// if mutex is locked, allow somebody else to unlock it
		asm jmp tryagain	// somebody has maybe unlocked it, try again
	}
}

*/
