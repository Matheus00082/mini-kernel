#ifndef FIFO_H
#define FIFO_H

#include"../control_structures/pcb.h"

typedef struct FIFO FIFO;

FIFO * fifo_create();
void fifo_push(void * f, PCB * data);
int fifo_size(void * f);
void * fifo_remove(void * f);
void fifo_destroy(void  * f);

#endif