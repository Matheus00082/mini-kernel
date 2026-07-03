#ifndef FIFO_H
#define FIFO_H

typedef struct FIFO FIFO;

FIFO * fifo_create(void (*destroy)(void *));
void fifo_push(void * f, void * data);
int fifo_size(void * f);
void * fifo_remove(void * f);
void fifo_destroy(void  * f);

#endif