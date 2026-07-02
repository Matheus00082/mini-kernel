#ifndef PCB_H
#define PCB_H

#define QUANTUM_TIME 500
typedef struct PCB PCB;

typedef enum{
    READY,
    RUNNING,
    FINISHED
} ProcessState;

typedef enum{
    FCFS = 1,
    ROUND_ROBIN = 2,
    PRIORITY = 3
} SchedulerType;

PCB* pcb_create(int pid, int process_duration, int priority, int amount_of_threads, int t0);
int pcb_compare(const void *process1, const void *process2);
void pcb_destroy(void * p);

#endif