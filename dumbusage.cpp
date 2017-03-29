//
// Created by omer on 3/29/17.
//

#include <iostream>
#include "uthreads.h"
void goo();
void boo();
void foo();
int main(){
    uthread_init(5000);
    uthread_spawn(foo);
    uthread_spawn(boo);
    uthread_spawn(goo);
    for(;;){
        pause();
    }
}



void foo(){
    int i = 0;
    while(i<100){
        cout << "in foo" << endl;
        ++i;
    }
    uthread_sync(3);
    foo();
}

void boo(){
    while(1){
        cout << "in boo" << endl;
    }
}

void goo() {
    while (1) {
        cout << "in goo" << endl;
    }
}