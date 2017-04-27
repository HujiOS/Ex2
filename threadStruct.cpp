//
// Created by Omer on 24/03/2017.
//
#ifndef SPTHREAD_C
#define SPTHREAD_C
#include <iostream>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <array>
#include "threadStruct.hpp"

using namespace std;
#define DEP_SIZE 100

#ifdef __x86_64__

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t spThread::trans_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */

address_t spThreads::trans_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%gs:0x18,%0\n"
			"rol    $0x9,%0\n"
			: "=g" (ret)
			  : "0" (addr));
	return ret;
}

#endif

int spThread::relies_on()
{
    return _relies_on;
}

/**
 * after running function, saving the location of the function
 */
int spThread::saveBuffer(){
    return sigsetjmp(_env,ARBITARY_VAL); // 0 if returns from saving, 1 if returning from longjmp.
}

/**
 * increase the quants and running the thread again
 */
void spThread::loadBuffer(){
    _quant++;
    siglongjmp(_env,ARBITARY_VAL);
}
/**
 * getter for status
 */
int spThread::getStatus(){
    return _status;
}
// we can assume that there is no two dependency for specific thread
// cause after we are calling sync we should stop the thread
void spThread::setDep(int tid){
    _relies_on = tid;
    _status = BLOCKED;
}

void spThread::sync(int tid){
//    cout << "syncing " << _tid << "to" << tid<< endl;
    _status = BLOCKED;
    _relies_on = tid;
}
/*
 * this function return true if status changed
 * false otherwise
 */
bool spThread::reSync(int tid){
    if(_relies_on == tid){
//        cout << "resyncing " << _tid<< "from" << tid << endl;
        _relies_on = -1;
        if(_blocked){
//            cout << "still in blocked cause its blocked"<< endl;
            return false;
        }
        _status = WAITING;
        return true;
    }
    return false;
}

void spThread::block(){
    _status = BLOCKED;
    _blocked = true;
}

/*
 * this function return true if status changed
 * false otherwise
 */
bool spThread::unblock(){

    if(!_blocked)
    {
        return false;
    }
    _blocked = false;
    if(_relies_on == -1){
        _status = WAITING;
        return true;
    }
    return false;
}

int spThread::tid(){
    return _tid;
}

int spThread::getQuant(){
    return _quant;
}


#endif
