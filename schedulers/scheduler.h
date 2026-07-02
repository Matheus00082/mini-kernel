#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include "../control_structures/pcb.h"

typedef struct Scheduler Scheduler;

typedef void (*func_ptr_ready_queue_insert)(void * ready_queue, PCB * pcb);
typedef void* (*func_ptr_ready_queue_remove)(void * ready_queue);
typedef int (*func_ptr_ready_queue_size)(void * ready_queue);
typedef void (*func_ptr_ready_queue_destroy)(void * ready_queue);

Scheduler * scheduler_create(SchedulerType type, FILE * log);
void scheduler_destroy(Scheduler * s);

#endif