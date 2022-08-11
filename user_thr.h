#ifndef USER_THREAD_H
#define USER_THREAD_H

#include "thread.h"
#include "locks.h"

class UserThread : public Thread {
public:
	UserThread(int _argc, char** _argv);
	~UserThread();

	int getReturnValue() { return ret; }

	Thread* clone() const {
		return new UserThread(argc, argv);
	}

protected:
	void run();

private:
	int argc, ret;
	char** argv;
};

#endif
