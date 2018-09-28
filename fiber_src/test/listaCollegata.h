
#define USERSPACE

#ifdef USERSPACE

#include <sys/types.h>
#include "fiber.h"

struct nodeFiber{
    fiber_t fiber;
    struct nodeFiber *next;
};

typedef struct nodeFiber* TypeFiberList;

void printList(TypeFiberList);
struct nodeFiber* addNode(TypeFiberList, fiber_t);
int sizeList(TypeFiberList);
void freeList(TypeFiberList);
fiber_t* findActiveFiber(TypeFiberList);

#endif /* USERSPACE */