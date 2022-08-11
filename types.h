#ifndef TYPES_H
#define TYPES_H

// bool
typedef int	 	bool;
#define false	0
#define true 	1

// nullptr
#define nullptr 0

// thread.h types
typedef unsigned long   StackSize;	// 32 bit
typedef unsigned int    Time; 		// x 55ms
typedef int             ID;

// stack
typedef unsigned Reg; 				// 16 bit
typedef unsigned Data; 				// 16 bit
#define MAX_STACK_SIZE 0x1000
#define MIN_STACK_SIZE 0x0400 		// ?

// psw
#define PSW_START_VALUE 0x0200 		// PSWI = 1

typedef void (*funPtr)(); 			// for wrapper

// interrupts
#define IVT_SIZE 256
typedef void interrupt (*intPtr)(...);

// timer
#define timerEntry 				0x08
#define oldTimerRoutineEntry 	0x60

#endif
