#include <stdio.h>
#include <stdlib.h>
#include "../data_structures/heap.h"
#include "../control_structures/pcb.h"
#include "PreemptivePriority.h"
#include "aux.h"

struct PP{
    tHeap *ready_queue;
    func_data_destroy data_destroy;
    int arrival_seq;
};

PP * priority_create(void (*destroy)(void *)){
    PP *p = calloc(1, sizeof(PP));
    p->ready_queue = heap_create(3); // 3 escolhido arbitráriamente
    p->data_destroy = destroy;
    p->arrival_seq = 0;
    return p;
}

void priority_push(void * p, void * data){
    PP *pp = (PP *)p;
    // critério de desempate considerando bem as casas decimais
    double key = (double)get_priority((PCB *)data) * 1000000.0 + pp->arrival_seq;
    pp->arrival_seq++;
    heap_push(pp->ready_queue, data, key);
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