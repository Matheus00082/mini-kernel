#ifndef RR_H
#define RR_H

#include"../control_structures/pcb.h"

typedef struct RR RR;

RR * round_robin_create();
void round_robin_push(void * r, PCB * data);
int round_robin_size(void * r);
void * round_robin_remove(void* r);
void round_robin_destroy(void * r);

#endif