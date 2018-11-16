#include <linux/list.h>

void add_fiber(struct fiber_t fiber_list, struct fiber_t* node){
	INIT_LIST_HEAD(&node -> list);
	list_add_tail(&(node->list),&(fiber_list.list))
}
