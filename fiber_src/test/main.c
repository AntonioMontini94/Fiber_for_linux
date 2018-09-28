#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "fiber.h"

fiber_t* list;
fiber_t* list1;

static int get_random_fiber(void) {
	return (random() % 8)+1;
}

void *test_a(void* a){
    int i;
    int* num;
    num = (int*) a;
    printf("-------------\n");
    for(i=1;i<=*num;i++){
        printf("    a - %d\n",i);
    }
    printf("-------------\n");
    int nn = get_random_fiber();
    printf("--> %d\n",nn);
    SwitchToFiber(list1);
}

void *test_b(int a){
    int i;
    
    for(i=1;i<(a/2)+1;i++){
        printf("    b - %d\n",i);
    }
    SwitchToFiber(list);
    for(i=(a/2)+1;i<=a;i++){
        printf("    b - %d\n",i);
    }
}

int main(){
    int a = 2;
    int i;
    list = (fiber_t*) malloc(sizeof(fiber_t*)*2);
    srandom(time(0));
    ConvertThreadToFiber();
    /*
    for(i=1;i<9;i++){
        if(!CreateFiber(1024,test_a,(void*)&a)){
            printf("%d\n",i);
            return 0;
        }
    }
    */

   if(!(list = CreateFiber(1024,test_a,(void*)&a))) return 0;
   list1 = list + sizeof(fiber_t*);
   if(!(list1 = CreateFiber(1024,test_a,(void*)&a))) return 0;
    test_b(a*2);
    return 0;
}