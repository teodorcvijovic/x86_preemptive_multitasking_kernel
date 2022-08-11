#include <iostream.h>

#include "user_thr.h"
#include "semaphor.h"
#include "event.h"
#include "global.h"
#include "user.h"

int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){

	int ret = 0;
	int _argc = argc;
	char** _argv = argv;

	Global::createGlobal();
	Idle::createIDLE();

	///////////

	Timer::init();

	cout<<"Timer initialized\n"<<endl;

	///////////

	UserThread* userThread = new UserThread(_argc, _argv);

	userThread->start();

	userThread->waitToComplete();
	ret = userThread->getReturnValue();

	delete userThread;

	///////////

	Timer::restore();

	cout<<"Timer restored\n"<<endl;

	return ret;

}
