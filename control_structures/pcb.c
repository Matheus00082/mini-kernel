#include "pcb.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "pthread.h"

struct PCB{
    int pid;
    int process_len;
    int remaining_time;
    int priority;
    int num_threads;
    int start_time;

    ProcessState state;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    pthread_t *thread_ids;
};
