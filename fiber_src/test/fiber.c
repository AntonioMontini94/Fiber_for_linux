#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include "fiber.h"

fiber_t* ConvertThreadToFiber(int stack_size){
    fiber_t new_fiber;
    fiber_l.list = (fiber_t*) malloc(sizeof(fiber_t)*2);
    fiber_l.count = 0;
    fiber_l.size = 2;
    printf("%d\n",stack_size);

    setjmp(new_fiber.context);
    new_fiber.fiber_id = fiber_l.count++;
    new_fiber.run = 1;
    new_fiber.thread_id = getpid();
    new_fiber.entry_point = NULL;
    new_fiber.params = NULL;
    new_fiber.activition_num = 1;
    
    fiber_l.list[0]=new_fiber;
    return &new_fiber;
}

fiber_t* CreateFiber(int stack_size, void (*handler), void* params){
    fiber_t new_fiber;

    if(fiber_l.count >= fiber_l.size-1) return NULL;

    setjmp(new_fiber.context);
    new_fiber.fiber_id = fiber_l.count++;
    new_fiber.run = 0;
    new_fiber.thread_id = getpid();
    new_fiber.activition_num = 0;
    new_fiber.entry_point = handler;
    new_fiber.params = params;

    return &new_fiber;
}

void SwitchToFiber(fiber_t* fib_to_run){

    if(fib_to_run->run == 0 && fib_to_run->activition_num == 0){
        return;
    }
}