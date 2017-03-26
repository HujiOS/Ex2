//
// Created by Omer on 24/03/2017.
//
#include <map>
#include <vector>
#include <iostream>
#include "uthreads.h"

#define ERR_TITLE "uthreads.cpp : "

#define INPUT_ERR 0

#define SUCC 0
#define FAIL -1

#define QUANTUM_USEC "Quantum value should be bigger than 0"
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











int uthread_init(int quantum_usecs){
    // check if the given number is positive
    if(quantum_usecs <= 0){
        error_log(INPUT_ERR, QUANTUM_USEC);
        return FAIL;
    }
//    we should define the calls handler here
//    _segActions.sa_handler


//  init!
    // creating the main thread and adding it to the list
    spThread* main = new spThread(nullptr, 0);
    _threads.insert(pair<int, spThread*>(0, main));
    _runningThread = main;
    main->setStatus(RUNNING);
    quantom_overall++;
    return SUCC;
}