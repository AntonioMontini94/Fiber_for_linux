#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "fiber.h"

struct arg_device{
	long thread_id;
	long process_id;
	void* (function);
	void* arg_function;
	int size_stack;
};

void* covertThreadToFiber(){
	long ret, fd;
	void* fiber_address;
	struct arg_device* arg = (struct arg_device*) malloc(sizeof(struct arg_device));
	arg->thread_id = (long) syscall(SYS_gettid);
	arg-> process_id = (long) getpid();
	arg->arg_function = NULL;
	
	fd = open("/dev/fiber", O_RDWR);            // Open the device with read/write access
	if (fd < 0){
	  perror("Failed to open the device...");
	  return (void*) &errno;
	}
	// Utilizzo  IOCT
	ret = ioctl(fd,CONVERT_THREAD,(unsigned long)arg);
	if (ret < 0){
	  perror("Failed to perform ioctl command.");
	  return (void*) &errno;
	}
	free(arg);
	// Casto il long int a void*, cioè il l'indirizzo della fibra 0
	fiber_address = (void*) ret;
	return fiber_address;
}

void* createFiber(int size_stack, void* function, void* args ){
	long ret, fd;
	void* fiber_address;
	struct arg_device* arg = (struct arg_device*) malloc(sizeof(struct arg_device));
	arg->thread_id = (long) syscall(SYS_gettid);
	arg-> process_id = (long) getpid();
	arg->arg_function = args;
	arg->function = function;
	arg->size_stack = size_stack;
	
	fd = open("/dev/fiber", O_RDWR);            // Open the device with read/write access
	if (fd < 0){
	  perror("Failed to open the device...");
	  return (void*) &errno;
	}
	// Utilizzo  IOCT
	ret = ioctl(fd,CREATE_FIBER,(unsigned long)arg);
	if (ret < 0){
	  perror("Failed to perform ioctl command.");
	  return (void*) &errno;
	}
	free(arg);
	// Casto il long int a void*, cioè il l'indirizzo della fibra 0
	fiber_address = (void*) ret;
	return fiber_address;
}
