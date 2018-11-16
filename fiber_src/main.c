#include <stdio.h>
#include "fiber.h"

int main(){
	void* fiber_0;
	fiber_0 = covertThreadToFiber();
	while(1);
	return 0;
}