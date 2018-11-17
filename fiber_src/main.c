#include <stdio.h>
#include "fiber.h"

int main(){
	void* fiber_0;
	void* fiber_1;
	fiber_0 = covertThreadToFiber();
	printf("Press ENTER to read back from the device...\n");
	getchar();
	fiber_1 = createFiber(100,NULL,NULL);
	//while(1);
	return 0;
}