

#include <iostream>
#include "uthreads.h"

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

int main(){
    uthread_init(1000);
    cout<<uthread_spawn(&foo)<<endl;
    cout<<uthread_spawn(&goo)<<endl;
    cout<<uthread_spawn(&boo)<<endl;
    while(1){

    }
}




