#ifndef DEQ_H
#define DEQ_H

typedef struct tDeq tDeq;

tDeq *deq_create(int allocated);
void deq_push_front(tDeq *deq, void *data);
void deq_push_back(tDeq *deq, void *data);
void *deq_pop_front(tDeq *deq);
void *deq_pop_back(tDeq *deq);
int deq_size(tDeq *deq);
int deq_allocated(tDeq *deq);
int deq_front(tDeq *deq);
int deq_back(tDeq *deq);
void *deq_get(tDeq *deq, int i);
void deq_destroy(tDeq *deq, void (*destroy)(void *));

#endif