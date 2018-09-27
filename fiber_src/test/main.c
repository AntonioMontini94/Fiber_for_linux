#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "fiber.h"

void *test_a(void* a){
    int i;
    int* num;
    num = (int*) a;
    printf("-------------\n");
    for(i=0;i<*num;i++){
        printf("    a - %d\n",i);
    }
    printf("-------------\n");
}

void *test_b(int a){
    int i;
    for(i=0;i<(a/2);i++){
        printf("    b - %d\n",i);
    }
    SwitchToFiber(&fiber_l.list[1]);
    for(i=(a/2)+1;i<a;i++){
        printf("    b - %d\n",i);
    }
}

int main(){
    fiber_t* this;
    fiber_t* new;
    int a = 10;
    this = ConvertThreadToFiber(1024);
    new = CreateFiber(1024,test_a,(void*)&a);
    test_b(20);
    return 0;
}