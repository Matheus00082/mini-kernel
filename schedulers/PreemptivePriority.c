#include <stdio.h>
#include <stdlib.h>
#include "../data_structures/heap.h"
#include "PreemptivePriority.h"
#include "aux.h"

struct PP{
    tHeap *ready_queue;
    func_data_destroy data_destroy;
};

PP * priority_create(void (*destroy)(void *)){
    PP *p = calloc(1, sizeof(PP));
    p->ready_queue = heap_create(3); // 3 escolhido arbitráriamente
    p->data_destroy = destroy;
    return p;
}

void priority_push(void * p, void * data){
    heap_push(((PP *)p)->ready_queue, data, 5);
}

void * priority_remove(void* p){
    return heap_pop(((PP *)p)->ready_queue);
}

int priority_size(void * p){
    return heap_size(((PP *)p)->ready_queue);
}

void priority_destroy(void * p){
    PP * pp = (PP *) p;
    heap_destroy(pp->ready_queue, pp->data_destroy);
    free(pp);
}