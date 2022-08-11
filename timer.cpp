#include "timer.h"

#include "kersem.h"

intPtr Timer::oldTimer = 0;

volatile bool contextSwitchOnDemand = false;
volatile Data lockCount = 0;

volatile Time countdown = defaultTimeSlice;

void tick(); // defined in USER.CPP

// temp regs for data transfer
volatile Reg tss;
volatile Reg tsp;
volatile Reg tbp;

void interrupt Timer::timer(...){

	if(!contextSwitchOnDemand){
			if(countdown > 0) --countdown;
			asm int oldTimerRoutineEntry;
			tick();
			KernelSem::notifyAllSems();
	}

	if(contextSwitchOnDemand || (countdown==0 && lockCount==0 && !running->unlimitedExeTime())){

		//if(lockFlag){

			//contextSwitchOnDemand=false;

			asm {
				mov tss, ss
				mov tsp, sp
				mov tbp, bp
			}
			running->ss = tss;
			running->sp = tsp;
			running->bp = tbp;
			running->myLockCount = lockCount;

			if(running->status == PCB::RUNNING){
				   running->status = PCB::READY;
				   Scheduler::put(running);
			}

			running = Scheduler::get();

			if (running==nullptr || running->status != PCB::READY) running = idlePCB;
			else running->status = PCB::RUNNING;

			lockCount = running->myLockCount;
			countdown = running->timeSlice;

			tss = running->ss;
			tsp = running->sp;
			tbp = running->bp;
			asm {
				mov ss, tss
				mov sp, tsp
				mov bp, tbp
			}
		//}
		//else contextSwitchOnDemand=true;
	}

	contextSwitchOnDemand = false;
}

void Timer::init(){
#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED(
			Timer::oldTimer = getvect(timerEntry);
			setvect(oldTimerRoutineEntry, Timer::oldTimer);
			setvect(timerEntry, Timer::timer);
	)
#endif
}

void Timer::restore(){
#ifndef BCC_BLOCK_IGNORE
	INT_DISABLED( setvect(timerEntry, Timer::oldTimer); )
#endif
}
