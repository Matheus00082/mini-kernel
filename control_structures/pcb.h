#ifndef PCB_H
#define PCB_H

#include "../schedulers/aux.h"
#define QUANTUM_TIME 500
typedef struct PCB PCB;

PCB* pcb_create(int pid, int process_duration, int priority, int amount_of_threads, int t0);
void pcb_destroy(void * p);

int pcb_compare(const void *process1, const void *process2);
void pcb_ready(PCB *);
void pcb_running(PCB *);
int pcb_finish(PCB *p);
int get_pid(PCB *);
int get_priority(PCB *p);
int get_start_time(PCB *);
int get_working_threads(PCB *p);
int get_last_burst(PCB *p);
void threads_join(PCB *p);
void pcb_start_threads(PCB *p);
void* thread_routine(void* arg);
#endif