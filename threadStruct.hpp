//
// Created by Omer on 24/03/2017.
//

#ifndef EX2_THREADSTRUCT_HPP
#define EX2_THREADSTRUCT_HPP
#define RUNNING 0
#define WAITING 1
#define BLOCKED 2
#define ENDED 0
#define PREEMPTED 1


#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
using namespace std;

class spThread{
public:
    spThread(function func):foo(func){
        foo = func;
    }
    // running the thread
    int run(){
        signal(SIGVTALARM, handleTimeout);
        if(env == NULL){
            foo();
        }
        siglongjmp(env,currRun);
        return ENDED;
    }
    // return status
    void getStatus(){
        return status;
    }
    // we can assume that there is no two dependency for specific thread
    // cause after we are calling sync we should stop the thread
    void setDep(int tid){
        dependency = tid;
        status = BLOCKED;
    }
    void freeDep(int tid){
        if(status == BLOCKED && tid == dependency){
            dependency = -1;
            status = WAITING;
        }
    }
    int handleTimeout(int sig_num){
        ret_val = sigsetjmp(env,1);
        return PREEMPTED;
    }

private:
    int status = WAITING;
    function foo;
    int dependency = -1;
    sigjmp_buf env;
    int currRun = 0;
};


#endif //EX2_PROCESSSTRUCT_HPP
