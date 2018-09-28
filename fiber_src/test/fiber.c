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

    //Alloco lo spazio per il primo elemento della lsita
	struct nodeFiber* node_f = malloc(sizeof(struct nodeFiber));
    fiber_t* new_fiber;
    new_fiber = (fiber_t*) malloc(sizeof(fiber_t));

    setjmp(new_fiber->context);
    new_fiber->fiber_id = sizeList(fiber_l)+1;
    new_fiber->run = 1;
    new_fiber->thread_id = getpid();
    new_fiber->function_work = NULL;
    new_fiber->params_function_work = NULL;
    new_fiber->activition_num = 1;
    new_fiber->finish = 0;

	//Inserisco ila prima fibra nella lista
	node_f -> fiber = *new_fiber;
	node_f -> next = NULL;

    fiber_l = node_f;

    return &(node_f->fiber);
}

fiber_t* CreateFiber(int stack_size, void* (*handler)(void*), void* params){
    fiber_t* new_fiber;
    new_fiber = (fiber_t*) malloc(sizeof(fiber_t));

    setjmp(new_fiber->context);
    new_fiber->fiber_id = sizeList(fiber_l)+1;
    new_fiber->run = 0;
    new_fiber->thread_id = getpid();
    new_fiber->activition_num = 0;
    new_fiber->function_work = handler;
    new_fiber->params_function_work = params;
    new_fiber->finish = 0;

    struct nodeFiber* ret = addNode(fiber_l,*new_fiber);

    return &(ret->fiber);
}

void SwitchToFiber(fiber_t* fib_to_run){
    fiber_t* this;
    int i;
    void* (*fun)(void*);

    this = findActiveFiber(fiber_l);

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
            fun = fib_to_run->function_work;           
            fun(fib_to_run->params_function_work);
            fib_to_run->run = 0;
            fib_to_run->finish = 1;
            longjmp(this->context,1);
            break;
        case 1:
            break;
    }
    
}