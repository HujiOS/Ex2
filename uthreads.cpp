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
#define FATAL_ERR 1

#define SUCC 0
#define FAIL -1

#define QUANTUM_USEC "Quantum value should be bigger than 0."
#define SIZE_LIMIT "We have reached the limit of 100 threads."
#define THREAD_NFOUND "Cannot find thread with the given tid."
#define STOP_MAIN "You cannot do this action on the main thread."
#define SYNC_ITSELF "A Thread cannot sync to itself"

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
static sigset_t _set;
static vector<sigset_t *> _origs;
struct itimerval _itTimer;
struct sigaction _segActions;
static bool isblocked = false;

void blockSignal();
void unblockSignal();
void reSyncBlocked(int tid);

void pauseMain(void){
    for(;;){}
}

/*
 * this function return the status about out library
 */
void printStatus()
{
    cout << endl;
    cout << endl;
    if (_runningThread)
    {
        cout << "Current thread is: " << _runningThread->tid() << endl;
    } else
    {
        cout << "running is null " << endl;
    }

    cout << "Current quant: " << quantom_overall << endl;
    cout << "all threads:" << endl;
    for(auto thread:_threads)
    {
        thread.second->status();
    }

    cout << endl << "in ready:";
    for(auto rthread:_readyThreads)
    {
        cout << rthread -> tid() << " ";
    }


    cout << endl << "in blocked: " << " ";
    for(auto bthread:_blockThreads)
    {
        cout <<endl;
        if(bthread -> relies_on() == -1)
        {
            cout << bthread -> tid() << " is blocked" <<endl;
        } else{
            cout << bthread -> tid() << " is synced to" << bthread -> relies_on()  <<endl;
        }


    }

    cout << endl;
    cout << endl;
    cout << endl;

}
/*
 * kind of a nice errors formatter, getting a code of the error and a string
 * and print it to cerr.
 */
void error_log(int pCode, string tCode){
    switch(pCode){
        case INPUT_ERR:
            cerr << ERR_TITLE << "Input error:" << tCode << endl;
            break;
        case FATAL_ERR:
            cerr << ERR_TITLE << "Fatal error:" << tCode << endl;
            break;
        default:
            break;
    }
}
/*
 * a function that switch threads its if the running thread exists it
 * returns it to the ready threads and if it isnt it just take the next one
 * from the ready list
 */

void switchThreads(int code)
{
    quantom_overall++;
    blockSignal();
    if(_runningThread != nullptr)
    {
        _runningThread->saveBuffer();// 1 is to be able to load it back on
        _readyThreads.push_back(_runningThread);
    }



    _runningThread = _readyThreads.front();

    _readyThreads.erase(_readyThreads.begin());
    reSyncBlocked(_runningThread -> tid());
//    printStatus();
    _runningThread->loadBuffer();
    if (setitimer (ITIMER_VIRTUAL, &_itTimer, NULL)) {
        error_log(FATAL_ERR,"setitimer error.");
    }
    unblockSignal();

}

/*
 * this function will find the next id to spawn according to the current threads
 * it will always take the lowest available id
 */
int resolveId()
{
    int idToReturn = 0;
    for (map<int, spThread*>::iterator iter = _threads.begin(); iter != _threads.end();
         idToReturn++, iter++)
    {
        if (iter->first != idToReturn)
        {
            break;
        }
    }
    return idToReturn;
}

/*
 * this function will terminate the main thread, free all the memory and etc.
 */
int mainthread_terminate(){
    for(auto thread : _threads){
        delete thread.second;
    }
    return SUCC;
}

/*
 * this function will get an id of tid and return the thread with that id.
 */
spThread* getThreadById(int tid){
    map<int, spThread*>::iterator it = _threads.find(tid);
    if(it == _threads.end()){
        return nullptr;
    }
    return it->second;
}

/*
 * this is a general function that deleting a specific thread pointer
 * from every block to generalize we dont want to determine which
 * lists contains the thread so we just remove it from both of them.
 */
void removeThreadFromBlocks(spThread* thread){
    for(auto rdy = _readyThreads.begin()
            ; rdy != _readyThreads.end()
            ; ++rdy){
        if(*rdy == thread){
            _readyThreads.erase(rdy);
            break;
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

/*
 * this function will block the SIGVTALARM signal
 */
void blockSignal(){
    if(isblocked)
    {
        return;
    }
    isblocked = true;

    sigprocmask(SIG_BLOCK, &_set, NULL);       //block ^^^ signals from now on
    return;
}
/*
 * this function will unblock the SIGVTALARM signal
 */
void unblockSignal(){
    if(!isblocked)
    {
        return;
    }
    isblocked = false;
    sigprocmask(SIG_UNBLOCK, &_set, NULL);
    return;
}

/*
 * this function will be called before every thread running and
 * will notify every blocked thread that thread #tid is running
 * if this message will cause a status change of the calle we will change
 * his position from blocked to ready.
 */
void reSyncBlocked(int tid){
    vector<int> l;
    for (int index = 0; index < _blockThreads.size(); ++index){
        if(_blockThreads[index]->reSync(tid)){
            l.push_back(_blockThreads[index]->tid());
        }
    }
    for(int i = 0; i< l.size();++i)
    {
        spThread* tmp = getThreadById(l[i]);
        removeThreadFromBlocks(tmp);
        _readyThreads.push_back(tmp);
    }

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

    sigemptyset(&_set);
    sigaddset(&_set, SIGVTALRM);

//  init var
    // creating the main thread and adding it to the list
    spThread* main = new spThread(&pauseMain, 0);
    _threads.insert(tPair(0, main));
    _runningThread = main;

    _segActions.sa_handler = &switchThreads;


    _itTimer.it_value.tv_sec = 0;		// first time interval, seconds part
    _itTimer.it_value.tv_usec = quantum_usecs;		// first time interval, microseconds part
    // configure the timer to expire every 3 sec after that.
    _itTimer.it_interval.tv_sec = 0;	// following time intervals, seconds part
    _itTimer.it_interval.tv_usec = quantum_usecs;	// following time intervals, microseconds part

    if (setitimer (ITIMER_VIRTUAL, &_itTimer, NULL)) {
        error_log(FATAL_ERR,"setitimer error.");
    }
    if (sigaction(SIGVTALRM, &_segActions,NULL) < 0) {
        error_log(FATAL_ERR,"sigaction error.");
    }
    raise(SIGVTALRM);
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
    if(nId == -1 || _threads.size() == MAX_THREAD_NUM){
        error_log(INPUT_ERR, SIZE_LIMIT);
        return FAIL;
    }
    spThread *tThread = new spThread(f, nId);
    _threads.insert(tPair(nId, tThread));
    _readyThreads.push_back(tThread);
    unblockSignal();
    return nId;
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
    if(tid == 0){
        mainthread_terminate();
        _exit(0);
    }
    spThread* thread = getThreadById(tid);
    if(thread == nullptr){
        error_log(INPUT_ERR, THREAD_NFOUND);
        return FAIL;
    }

    if(_runningThread == thread)
    {
        _runningThread = nullptr;
    }
    blockSignal();
    reSyncBlocked(tid);
    _threads.erase(tid);
    removeThreadFromBlocks(thread);
    delete thread;
    unblockSignal();
    raise(SIGVTALRM);//switchThreads(0);   //Do we need the number? I think not. ##### A BETTER WAY TO DO THIS? SIGNALS?
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
    if(thread == nullptr){
        return FAIL;
    }
    if(tid == 0){
        error_log(FATAL_ERR, STOP_MAIN);
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    thread -> block();
    removeThreadFromBlocks(thread);
    _blockThreads.push_back(thread);

    unblockSignal();
    if(tid == _runningThread->tid()){
        _runningThread -> saveBuffer();
        _runningThread = nullptr;
        raise(SIGVTALRM);//switchThreads(1); //##### A BETTER WAY TO DO THIS? SIGNALS?
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
        error_log(INPUT_ERR,THREAD_NFOUND);
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    if(thread -> unblock()){
        removeThreadFromBlocks(thread);
        _readyThreads.push_back(thread);
    }
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
    if(thread == nullptr){
        error_log(INPUT_ERR,THREAD_NFOUND);
        return FAIL;
    }
    if(_runningThread->tid() == 0){
        error_log(FATAL_ERR,STOP_MAIN);
        return FAIL;
    }
    if(_runningThread->tid() == tid){
        error_log(FATAL_ERR,SYNC_ITSELF);
        return FAIL;
    }
    // remove the block from ready and from block lists.
    blockSignal();
    _runningThread->sync(tid);
    removeThreadFromBlocks(_runningThread);
    _blockThreads.push_back(_runningThread);
    _runningThread -> saveBuffer();
    _runningThread = nullptr;
    unblockSignal();
    raise(SIGVTALRM);//switchThreads(0); //##### A BETTER WAY TO DO THIS? SIGNALS?
    return SUCC;
}

/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid(){
    return _runningThread->tid();
}

/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums(){
    return quantom_overall;
}

/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid){
    blockSignal();
    spThread *thread = getThreadById(tid);
    if(thread == nullptr){
        error_log(INPUT_ERR,THREAD_NFOUND);
        unblockSignal();
        return FAIL;
    }
    unblockSignal();
    return thread->getQuant();
}

