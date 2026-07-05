#ifndef AUX_H
#define AUX_H

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

typedef enum{
    UNCONCLUDED = 0,
    CONCLUDED = 1
} SchedulerState;

typedef enum{
    MONO = 0,
    MULTI = 1
} AuxSchedulerType;

typedef void (*func_queue_insert)(void * ready_queue, void * pcb);
typedef void* (*func_queue_remove)(void * ready_queue);
typedef int (*func_queue_size)(void * ready_queue);
typedef void (*func_queue_destroy)(void * ready_queue);
typedef void (*func_data_destroy)(void * data);

#endif