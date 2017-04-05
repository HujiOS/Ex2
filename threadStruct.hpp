//
// Created by Omer on 24/03/2017.
//
#ifndef SPTHREAD_H
#define SPTHREAD_H
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <array>


#define SECOND 1000000
#define STACK_SIZE 4096
#define ARBITARY_VAL 1
#define RUNNING 0
#define WAITING 1
#define BLOCKED 2
#define ENDED 0
#define PREEMPTED 1
using namespace std;
#define DEP_SIZE 100

#ifdef __x86_64__
typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */

#endif

class spThread{
public:
    static address_t trans_address(address_t addr);
    /*
     * creating new thread, the lower block of code was taken as is from demo_jmp.c
     * on windows I C syntax error..
     */
    spThread(void (*f)(void), int tid):_relies_on(-1),_status(WAITING),_quant(0),_tid(tid),_blocked(false){
        address_t sp, pc;
        _needs_me = new int[DEP_SIZE];

        sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
        pc = (address_t)f;
        sigsetjmp(_env, ARBITARY_VAL);
        (_env->__jmpbuf)[JB_SP] = trans_address(sp);
        (_env->__jmpbuf)[JB_PC] = trans_address(pc);
        sigemptyset(&_env->__saved_mask);
    }

    /**
     * small simple deleter
     */
    ~spThread(){
        delete[](_needs_me);
    }

    /**
     * after running function, saving the location of the function
     */
    int saveBuffer();

    /**
     * increase the quants and running the thread again
     */
    void loadBuffer();
    /**
     * getter for status
     */
    int getStatus();
    // we can assume that there is no two dependency for specific thread
    // cause after we are calling sync we should stop the thread
    void setDep(int tid);

    void sync(int tid);
    /*
     * this function return true if status changed
     * false otherwise
     */
    bool reSync(int tid);

    void block();

    /*
     * this function return true if status changed
     * false otherwise
     */
    bool unblock();

    int tid();


    void setStatus(int status);

private:
    int _relies_on;
    int _status;
    int _quant;
    int _tid;
    bool _blocked;
    sigjmp_buf _env;
    int* _needs_me;
    char _stack[STACK_SIZE];

};

#endif
