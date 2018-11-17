#define CONVERT_THREAD 1
#define CREATE_FIBER 2

void* covertThreadToFiber(void);
void* createFiber(int size_stack, void* function, void* args );