#include "pcb.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

struct PCB
{
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

PCB *pcb_create(int pid, int process_len, int priority, int num_threads, int start_time)
{
    PCB *p = calloc(1, sizeof(PCB));

    p->pid = pid;
    p->process_len = process_len;
    p->remaining_time = process_len;
    p->priority = priority;
    p->num_threads = num_threads;
    p->start_time = start_time;
    p->state = READY;

    if (pthread_mutex_init(&p->mutex, NULL) != 0)
    {
        fprintf(stderr, "Erro: falha ao inicializar mutex\n");
        free(p);
        return NULL;
    }

    if (pthread_cond_init(&p->cv, NULL) != 0)
    {
        fprintf(stderr, "Erro: falha ao inicializar condition variable\n");
        pthread_mutex_destroy(&p->mutex);
        free(p);
        return NULL;
    }

    p->thread_ids = calloc(num_threads, sizeof(pthread_t));

    return p;
}

int pcb_compare(const void *process1, const void *process2)
{
    PCB *p1 = *(PCB **)process1;
    PCB *p2 = *(PCB **)process2;

    if (p1->start_time > p2->start_time)
    {
        return 1;
    }
    else if (p1->start_time < p2->start_time)
    {
        return -1;
    }
    else
    {
        if (p1->pid < p2->pid) return -1;
        if (p1->pid > p2->pid) return 1;
        return 0;
    }
}

void pcb_destroy(void *pcb)
{
    PCB *p = (PCB *)pcb;
    free(p->thread_ids);
    pthread_cond_destroy(&p->cv);
    pthread_mutex_destroy(&p->mutex);
    free(p);
}