#include "deq.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct tDeq
{
    void **data;
    int allocated;
    int size;
    int front;
    int back;
} tDeq;

tDeq *deq_create(int allocated)
{
    tDeq *d = (tDeq *)calloc(1, sizeof(tDeq));
    d->data = (void **)calloc(allocated, sizeof(void *));
    d->allocated = allocated;
    d->size = 0;
    d->front = d->back = 0;

    return d;
}

void deq_push_front(tDeq *deq, void *data)
{
    int start = deq->front - 1;
    if (start < 0)
    {
        start = deq->allocated - 1;
    }
    deq->data[deq->front] = data;
    deq->size++;
}

void deq_push_back(tDeq *deq, void *data)
{
    deq->data[deq->back] = data;
    deq->back = (deq->back + 1) % deq->allocated;
    deq->size++;
}

void *deq_pop_front(tDeq *deq)
{
    void *d = deq->data[deq->front];
    deq->front = (deq->front + 1) % deq->allocated;
    deq->size--;
    return d;
}

void *deq_pop_back(tDeq *deq)
{
    int end = deq->back - 1;
    if (end < 0)
    {
        end = deq->allocated - 1;
    }
    void *d = deq->data[deq->back];
    deq->size--;
    return d;
}

int deq_size(tDeq *deq) { return deq->size; }

int deq_allocated(tDeq *deq) { return deq->allocated; }

int deq_front(tDeq *deq) { return deq->front; }

int deq_back(tDeq *deq) { return deq->back; }

void *deq_get(tDeq *deq, int i) { return deq->data[i]; }

void deq_destroy(tDeq *deq, void (*destroy)(void *))
{
    if (destroy)
    {
        for (int i = 0; i < deq->size; i++)
        {
            destroy(deq->data[i]);
        }
    }

    free(deq->data);
    free(deq);
}