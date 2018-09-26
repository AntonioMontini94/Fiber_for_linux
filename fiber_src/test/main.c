#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "fiber.h"

void somma(int a){
    printf("%d\n",a+a);
}

int main(){
    fiber_t* this;
    fiber_t* new;
    int a = 5;
    this = ConvertThreadToFiber(1024);
    new = CreateFiber(1024,somma,(void*)&a);


    return 0;
}