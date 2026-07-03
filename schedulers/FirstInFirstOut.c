#include <stdio.h>
#include <stdlib.h>
#include "../data_structures/deq.h"
#include "FirstInFirstOut.h"
#include "aux.h"

struct FIFO{
    tDeq *ready_queue;
    func_data_destroy data_destroy;
};

FIFO * fifo_create(void (*destroy)(void *)){
    FIFO *f = calloc(1, sizeof(FIFO));
    f->ready_queue = deq_create(3); // 3 escolhido arbitráriamente
    f->data_destroy = destroy;
    return f;
}

void fifo_push(void * f, void * data){
    deq_push_back(((FIFO*)f)->ready_queue, data);
}

int fifo_size(void * f){
    return deq_size(((FIFO *)f)->ready_queue);
}

void * fifo_remove(void * f){
    return deq_pop_front(((FIFO *)f)->ready_queue);
}

void fifo_destroy(void  * f){
    FIFO *fifo = (FIFO *)f;
    deq_destroy(fifo->ready_queue, fifo->data_destroy);
    free(f);
}