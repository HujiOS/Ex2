//
// Created by Omer on 24/03/2017.
//
#include <map>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "uthreads.h"

#define ERR_TITLE "uthreads.cpp : "

#define INPUT_ERR 0

#define SUCC 0
#define FAIL -1

#define QUANTUM_USEC "Quantum value should be bigger than 0"

#define tPair pair<int, spThread*>
/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
static map<int, spThread*> _threads;
static vector<spThread*> _readyThreads;
static vector<spThread*> _blockThreads;
static spThread* _runningThread;
static int quantom_overall = 0;
struct itimerval _itTimer;
struct sigaction _segActions;

void error_log(int pCode, string tCode){
    switch(pCode){
        case INPUT_ERR:
            cerr << ERR_TITLE << "Input error:" << tCode << endl;
            break;
        default:
            break;
    }
}


int resolveId(){
    int id = 1;
    try {
        for (id; id < 101; id++) {
            if (_threads.at(id) != nullptr) {
                return id;
            }
        }
    }
    catch (out_of_range){
        return id;
    }
    // cant find the next id somehow..
    return -1;
}


int uthread_terminate(){
    for(auto thread : _threads){
        delete(thread);
    }
    return SUCC;
}

spThread* getThreadById(int tid){
    map<int, spThread*>::iterator it = _threads.find(tid);
    if(it == _threads.end()){
        return nullptr;
    }
    return it->second;
}

void removeThreadFromBlocks(spThread* thread){
    for(auto rdy = _readyThreads.begin()
            ; rdy != _readyThreads.end()
            ; ++rdy){
        if(*rdy == thread){
            _readyThreads.erase(rdy);
            return;
        }
    }
    for(auto blk = _blockThreads.begin()
            ; blk != _blockThreads.end()
            ; ++blk){
        if(*blk == thread){
            _blockThreads.erase(blk);
            break;
        }
    }
}


void blockSignal(){
    return;
}

void unblockSignal(){
    return;
}






///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////Library functions////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/*
 * this function need to be called to init the uthread library
 * @return SUCCESS if init successfuly, otherwise FAIL
 */
int uthread_init(int quantum_usecs){
    // check if the given number is positive
    if(quantum_usecs <= 0){
        error_log(INPUT_ERR, QUANTUM_USEC);
        return FAIL;
    }
//    we should define the calls handler here
//    _segActions.sa_handler


//  init var
    // creating the main thread and adding it to the list
    spThread* main = new spThread(nullptr, 0);
    _threads.insert(tPair(0, main));
    _runningThread = main;
    main->setStatus(RUNNING);
    quantom_overall++;
    return SUCC;
}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void)){
    blockSignal();
    int nId = resolveId();
    // cannot allocate id
    if(nId == -1 || _threads.size() == 100){
        cout << "exit on spawning for thread id : "<< nId << endl;
        return FAIL;
    }
    spThread *tThread = new spThread(f, nId);
    _threads.insert(tPair(nId,tThread));
    _readyThreads.push_back(tThread);
    unblockSignal();
    return SUCC;
}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered as an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid){
    blockSignal();
    if(tid == 0){
        uthread_terminate();
        _exit(0);
    }
    spThread* thread = getThreadById(tid);
    if(thread == nullptr){
        return FAIL;
    }
    _threads.erase(tid);
    removeThreadFromBlocks(thread);
    delete(thread);
    unblockSignal();
    return SUCC;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid){
    // check if this thread exists
    spThread* thread = getThreadById(tid);
    if(thread == nullptr || tid == 0){
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    thread->block(tid);
    removeThreadFromBlocks(thread);
    _blockThreads.push_back(thread);
    unblockSignal();
    if(tid == _runningThread->tid()){
        // in case thread blocks himself
        //TODO Rescheduele
    }
    return SUCC;
}

/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid){
    spThread* thread = getThreadById(tid);
    if(thread == nullptr){
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    thread->release();
    removeThreadFromBlocks(thread);
    _readyThreads.push_back(thread);
    unblockSignal();
    return SUCC;
}

/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will move to RUNNING state (i.e.right after the next time that
 * thread tid will stop running, the calling thread will be resumed
 * automatically). If thread with ID tid will be terminated before RUNNING
 * again, the calling thread should move to READY state right after thread
 * tid is terminated (i.e. it won’t be blocked forever). It is considered
 * as an error if no thread with ID tid exists or if the main thread (tid==0)
 * calls this function. Immediately after the RUNNING thread transitions to
 * the BLOCKED state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid){
    spThread* thread = getThreadById(tid);
    _runningThread->block(tid);
    if(thread == nullptr || _runningThread->tid() == 0){
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    _readyThreads.push_back(_runningThread);
    // TODO SCHEDUELING DECISION
    unblockSignal();
    return SUCC;
}

