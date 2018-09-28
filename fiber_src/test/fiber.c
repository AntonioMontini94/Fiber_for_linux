#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <unistd.h>
#include "fiber.h"

int get_fiber_id(fiber_t* fiber){
    return fiber->fiber_id;
}

fiber_t* ConvertThreadToFiber(){
    fiber_t* new_fiber;
    fiber_list* new_list;

    new_fiber = (fiber_t*) malloc(sizeof(fiber_t));
    new_list = (fiber_list*) malloc(sizeof(fiber_list));
    fiber_l = *new_list;
    fiber_l.list = (fiber_t*) malloc(sizeof(fiber_t)*FIBER_NUM);
    fiber_l.count = 0;
    fiber_l.size = FIBER_NUM;

    setjmp(new_fiber->context);
    new_fiber->fiber_id = fiber_l.count;
    new_fiber->run = 1;
    new_fiber->thread_id = getpid();
    new_fiber->entry_point = NULL;
    new_fiber->params = NULL;
    new_fiber->activition_num = 1;
    new_fiber->finish = 0;

    fiber_l.list[0] = *new_fiber;
    fiber_l.count++;
    //printf("%d------------\n",fiber_l.count);
    return &fiber_l.list[0];
}

fiber_t* CreateFiber(int stack_size, void* (*handler)(void*), void* params){
    fiber_t* new_fiber;
    new_fiber = (fiber_t*) malloc(sizeof(fiber_t));

    //printf("%d > %d\n",fiber_l.count,fiber_l.size-1);

    if(fiber_l.count > fiber_l.size-1) return NULL;

    setjmp(new_fiber->context);
    new_fiber->fiber_id = fiber_l.count;
    new_fiber->run = 0;
    new_fiber->thread_id = getpid();
    new_fiber->activition_num = 0;
    new_fiber->entry_point = handler;
    new_fiber->params = params;
    new_fiber->finish = 0;

    fiber_l.list[fiber_l.count] = *new_fiber;
    
    return &fiber_l.list[fiber_l.count++];
}

void SwitchToFiber(fiber_t* fib_to_run){
    fiber_t* this;
    int i;
    void* (*fun)(void*);

    for(i=0 ; i<FIBER_NUM ; i++) if(fiber_l.list[i].run==1) this = &fiber_l.list[i];

    if(this==fib_to_run) return;
    if(fib_to_run->finish == 1) return;

    switch(setjmp(this->context)){
        case 1:
            this->run = 1;
            this->activition_num++;
            return;
        default:
            this->run = 0;
    }

    switch(setjmp(fib_to_run->context)){
        case 0:
            fib_to_run->run = 1;
            fib_to_run->activition_num++;
            fun = fib_to_run->entry_point;           
            fun(fib_to_run->params);
            fib_to_run->run = 0;
            fib_to_run->finish = 1;
            longjmp(this->context,1);
            break;
        case 1:
            break;
    }
    
}