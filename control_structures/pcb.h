#ifndef PCB_H
#define PCB_H

typedef struct PCB PCB;

typedef enum{
    READY,
    RUNNING,
    FINISHED
} ProcessState;

PCB* pcb_create(int pid, int process_duration, int priority, int amount_of_threads, int t0);
int pcb_compare(const void *process1, const void *process2);
void pcb_destroy(void * p);

#endif