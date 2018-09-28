#include "fiber.h"
#include "listaCollegata.h"

void printList(TypeFiberList list){
	while(list != NULL){
		printf("ID = %d\n",list->fiber.fiber_id);
		printf("=============\n");
		list = list->next;
	}
}

struct nodeFiber* addNode(TypeFiberList list, fiber_t f){

	while(list->next != NULL){ 
		list = list -> next;
	}

	struct nodeFiber* nuovoNodo = malloc(sizeof(struct nodeFiber));
	nuovoNodo -> fiber = f;
	nuovoNodo -> next = NULL;

	list->next = nuovoNodo;
	return nuovoNodo;
}

int sizeList(TypeFiberList list){
	int n = 0;
	while(list != NULL){
		list = list->next;
		n++;
	}
	return n;
}

void deallocaLista(TypeFiberList list){
	struct nodeFiber* nodoScan;
	while(list != NULL){
		nodoScan = list->next;
		free(&(list->fiber));
		list = nodoScan;
	}
}

fiber_t* findActiveFiber(TypeFiberList list){
	while(list != NULL){
		if((list->fiber).run == 1){
			return &list->fiber;
		}
		list = list->next;
	}
	return NULL;
}