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
    spThread(void (*f)(void), int tid):_relies_on(-1),_status(WAITING),_tid(tid),_blocked(false){
        address_t sp, pc;
        _quant = 0;
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
    ~spThread(){}

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
     * this function return true if status changed (if the thread wasnt blocked) by thread.block()
     * false otherwise
     */
    bool reSync(int tid);

    /*
     * this function blocking the current Thread
     * it raise the flag "blocked"
     */
    void block();

    /*
     * this function return true if status changed
     * false otherwise
     */
    bool unblock();


    /**
     * returns the dependency of this thread
     */
    int relies_on();
    /*
     * returns the tid of the current thread
     */
    int tid();

    /*
     * return the number of qunatoms that passed
     */
    int getQuant();
	
	/*
     * print Thread status
     */
    void status();

private:
    // represent the tid that this thread synced to it.
    int _relies_on;

    // represent the current thread status
    int _status;

    // represent the number of quant that I have runned so far
    int _quant;

    // represent the thread thread Id
    int _tid;

    // bool says if im blocked or not.
    bool _blocked;

    // saving the environment (stack pointer and pc pointer)
    sigjmp_buf _env;

    // represent the stack location for this thread
    char _stack[STACK_SIZE];

};

#endif
