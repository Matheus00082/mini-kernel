#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include "../control_structures/pcb.h"

typedef struct Scheduler Scheduler;

typedef struct AuxScheduler
{
    Scheduler *s;
    int ID;
    AuxSchedulerType type;
} AuxScheduler;

AuxScheduler *aux_scheduler_create(Scheduler *s, int id, AuxSchedulerType type);
void aux_scheduler_destroy(AuxScheduler *);

Scheduler * scheduler_create(SchedulerType type, FILE * log);
void *scheduler_run(void *);
void scheduler_push(Scheduler *s, PCB *p);
void scheduler_finish(Scheduler *s, int cores);
void scheduler_destroy(Scheduler * s);

#endif