#pragma once

#define USERSPACE

#ifdef USERSPACE

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define FIBER_NUM 2

typedef struct {
    jmp_buf context;
    void* (*entry_point)(void*);
    void* params;
    int fiber_id;
    short run;
    pid_t thread_id;
    int activition_num;
} fiber_t;

typedef struct {
    int count;
    int size;
    fiber_t* list;
} fiber_list;

fiber_list fiber_l;

fiber_t* ConvertThreadToFiber(int);
fiber_t* CreateFiber(int stack_size, void* (*handler)(void*), void* params);
void SwitchToFiber(fiber_t*);


#endif /* USERSPACE */