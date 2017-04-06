

#include <iostream>
#include "uthreads.h"
int f = 0;
int g = 0;
int b = 0;
void foo(){
    int i = 0;
    while (i < 3) {
        cout << "in goo" << endl;
        usleep(900);
        ++i;
    }
    uthread_terminate(f);
}

void boo(){
    int i = 0;
    while (i < 3) {
        cout << "in goo" << endl;
        usleep(900);
        ++i;
    }
    uthread_terminate(b);
}

void goo() {
    int i = 0;
    while (i < 3) {
        cout << "in goo" << endl;
        usleep(900);
        ++i;
    }
    uthread_terminate(g);
}

int main(){
    uthread_init(5000);
    f = uthread_spawn(&foo);
    g = uthread_spawn(&goo);
    b = uthread_spawn(&boo);
    for(;;){
    }
}




