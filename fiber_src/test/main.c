#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "fiber.h"

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
    SwitchToFiber(&fiber_l.list[nn]);
}

void *test_b(int a){
    int i;
    for(i=1;i<(a/2)+1;i++){
        printf("    b - %d\n",i);
    }
    SwitchToFiber(&fiber_l.list[1]);
    for(i=(a/2)+1;i<=a;i++){
        printf("    b - %d\n",i);
    }
}

int main(){
    int a = 2;
    int i;
    srandom(time(0));
    ConvertThreadToFiber();
    for(i=1;i<9;i++){
        if(!CreateFiber(1024,test_a,(void*)&a)){
            printf("%d\n",i);
            return 0;
        }
    }
    test_b(a*2);
    return 0;
}