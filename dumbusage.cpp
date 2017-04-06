

#include <iostream>
#include "uthreads.h"
int f = 0;
int g = 0;
int b = 0;
void foo(){
    int i = 0;
    while (i < 35) {
        cout << "in foo" << endl;
        sleep(1);
        ++i;
    }
    uthread_terminate(f);
}

void boo(){
    int i = 0;
    while (i < 35) {
        cout << "in boo" << endl;
        sleep(1);
        ++i;
    }
    uthread_terminate(b);
}

void goo() {
    int i = 0;
    while (i < 35) {
        cout << "in goo" << endl;
        sleep(1);
        ++i;
    }
    uthread_terminate(g);
}

int main(){
    uthread_init(1);
    f = uthread_spawn(&foo);
    b = uthread_spawn(&goo);
    g = uthread_spawn(&boo);
    cout << f << b << g << endl;
    for(;;){
    }
}




