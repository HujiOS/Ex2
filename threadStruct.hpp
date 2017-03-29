//
// Created by Omer on 24/03/2017.
//

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <array>

#define SECOND 1000000
#define STACK_SIZE 4096


sigjmp_buf env[2];

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
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

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}


#endif

#define ARBITARY_VAL 1
#define RUNNING 0
#define WAITING 1
#define BLOCKED 2
#define ENDED 0
#define PREEMPTED 1
using namespace std;
#define DEP_SIZE 100




class spThread{
public:
    /*
     * creating new thread, the lower block of code was taken as is from demo_jmp.c
     * on windows I C syntax error..
     */
    spThread(void (*f)(void), int tid):_relies_on(-1),_status(WAITING),_quant(0),_tid(tid){
        address_t sp, pc;
        _stack = new char[STACK_SIZE];
        _needs_me = new int[DEP_SIZE];

        sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
        pc = (address_t)f;
        sigsetjmp(_env, ARBITARY_VAL);
        (_env->__jmpbuf)[JB_SP] = translate_address(sp);
        (_env->__jmpbuf)[JB_PC] = translate_address(pc);
        sigemptyset(&_env->__saved_mask);
    }

    /**
     * small simple deleter
     */
    ~spThread(){
        delete[](_stack);
        delete[](_needs_me);
    }

    /**
     * after running function, saving the location of the function
     */
    int saveBuffer(){
        return sigsetjmp(_env,ARBITARY_VAL);
    }

    /**
     * increase the quants and running the thread again
     */
    void loadBuffer(){
        _quant++;
        siglongjmp(_env,ARBITARY_VAL);
    }
    /**
     * getter for status
     */
    int getStatus(){
        return _status;
    }
    // we can assume that there is no two dependency for specific thread
    // cause after we are calling sync we should stop the thread
    void setDep(int tid){
        _relies_on = tid;
        _status = BLOCKED;
    }

    void block(int tid){
        _status = BLOCKED;
        _relies_on = tid;
    }

    void release(){
        _status = WAITING;
        _relies_on = -1;
    }

    int tid(){
        return _tid;
    }


    void setStatus(int status){
        _status = status;
    }

private:
    int _status;
    int _relies_on;
    sigjmp_buf _env;
    int _quant;
    int* _needs_me;
    char* _stack;
    int _tid;
};
