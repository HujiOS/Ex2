

#include <iostream>
#include "uthreads.h"

void foo(){
    int i = 0;
    while(i<100){
        cout << "in foo" << endl;
        ++i;
        sleep(900);
    }
}

void boo(){
    while(1){
        cout << "in boo" << endl;
        sleep(900);
    }
}

void goo() {
    while (1) {
        cout << "in goo" << endl;
        sleep(900);
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




