#include "user_thr.h"

int userMain(int argc, char* argv[]);

UserThread::UserThread(int _argc, char** _argv){
    argc = _argc;
    argv = _argv;
    ret = 0;
}

UserThread::~UserThread(){
    waitToComplete();
}

void UserThread::run(){
    ret = userMain(argc, argv);
}
