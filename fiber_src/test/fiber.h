#pragma once

#define USERSPACE

#ifdef USERSPACE

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define FIBER_NUM 10

typedef struct {
    jmp_buf context;
    void* (*entry_point)(void*);
    void* params;
    int fiber_id;
    pid_t thread_id;
    int activition_num;
    short run;
    short finish;   
} fiber_t;

typedef struct {
    fiber_t* list;
    int count;
    int size;
} fiber_list;

fiber_list fiber_l;

fiber_t* ConvertThreadToFiber();
fiber_t* CreateFiber(int stack_size, void* (*handler)(void*), void* params);
void SwitchToFiber(fiber_t*);


#endif /* USERSPACE */