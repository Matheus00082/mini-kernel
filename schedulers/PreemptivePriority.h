#ifndef PP_H
#define PP_H

#include"../control_structures/pcb.h"

typedef struct PP PP;

PP * priority_create();
void priority_push(void * r, PCB * data);
void * priority_remove(void* r);
int priority_size(void * p);
void priority_destroy(void * r);

#endif