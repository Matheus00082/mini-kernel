#include <stdio.h>
#include <stdlib.h>
#include "../data_structures/deq.h"
#include "RoundRobin.h"
#include "aux.h"

struct RR {
    tDeq *ready_queue;
    func_data_destroy data_destroy;
};

RR * round_robin_create(void (*destroy)(void *)){
    RR *r = calloc(1, sizeof(RR));
    r->ready_queue = deq_create(3); // 3 escolhido arbitráriamente
    r->data_destroy = destroy;
    return r;
}

void round_robin_push(void * r, void * data){
    return deq_push_back(((RR *)r)->ready_queue, data);
}

int round_robin_size(void * r){
    return deq_size(((RR *)r)->ready_queue);
}

void * round_robin_remove(void* r){
    return deq_pop_front(((RR *)r)->ready_queue);
}

void round_robin_destroy(void * r){
    RR *rr = (RR *)r;
    deq_destroy(rr->ready_queue, rr->data_destroy);
    free(r);
}