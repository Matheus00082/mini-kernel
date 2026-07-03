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
    int quantum_left;

    ProcessState state;
    pthread_mutex_t mutex_1;
    pthread_mutex_t mutex_2;
    pthread_cond_t cv_1;
    pthread_cond_t cv_2;
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
    p->quantum_left = 0;
    p->state = READY;

    if (pthread_mutex_init(&p->mutex_1, NULL) != 0)
    {
        fprintf(stderr, "Erro: falha ao inicializar mutex\n");
        free(p);
        return NULL;
    }

    if (pthread_cond_init(&p->cv_1, NULL) != 0)
    {
        fprintf(stderr, "Erro: falha ao inicializar condition variable\n");
        pthread_mutex_destroy(&p->mutex_1);
        free(p);
        return NULL;
    }

    p->thread_ids = calloc(num_threads, sizeof(pthread_t));

    return p;
}

void pcb_destroy(void *pcb)
{
    PCB *p = (PCB *)pcb;
    free(p->thread_ids);
    pthread_cond_destroy(&p->cv_1);
    pthread_mutex_destroy(&p->mutex_1);
    free(p);
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

void pcb_ready(PCB *p)
{
    pthread_mutex_lock(&p->mutex_1);
    p->state = READY;
    pthread_cond_signal(&p->cv_1);
    pthread_mutex_unlock(&p->mutex_1);
}

void pcb_running(PCB *p)
{
    pthread_mutex_lock(&p->mutex_1);
    p->state = RUNNING;
    pthread_cond_signal(&p->cv_1);
    pthread_mutex_unlock(&p->mutex_1);
}

int pcb_finish(PCB *p)
{
    pthread_mutex_lock(&p->mutex_2);

    while (p->quantum_left == 0)
    {
        pthread_cond_wait(&p->cv_2, &p->mutex_2);
    }

    p->quantum_left = 0;
    pthread_mutex_unlock(&p->mutex_2);

    return (p->state == FINISHED);
}

int get_pid(PCB *p)
{
    return p->pid;
}

int get_priority(PCB *p)
{
    return p->priority;
}

int get_start_time(PCB *p)
{
    return p->start_time;
}

void threads_join(PCB *p)
{
    for (int i = 0; i < p->num_threads; i++)
    {
        pthread_join(p->thread_ids[i], NULL);
    }
    
}