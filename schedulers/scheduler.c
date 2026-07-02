#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "scheduler.h"
#include "FirstInFirstOut.h"
#include "PreemptivePriority.h"
#include "RoundRobin.h"

struct Scheduler{
    void * ready_queue;
    SchedulerType type;
    int finish;
    int quantum;
    FILE * log;
    pthread_mutex_t lock;
    pthread_cond_t cond;

    func_ptr_ready_queue_insert ready_queue_insert_fn;
    func_ptr_ready_queue_remove ready_queue_remove_fn;
    func_ptr_ready_queue_size ready_queue_size_fn;
    func_ptr_ready_queue_destroy ready_queue_destroy_fn;
};

Scheduler * scheduler_create(SchedulerType type, FILE * log) {
    Scheduler * s = (Scheduler*)calloc(1, sizeof(Scheduler));

    s->quantum = QUANTUM_TIME;
    s->log = log;

    // Define as funções da fila de prontos a depender do algoritmo
    if(type == FCFS) {
        s->ready_queue = FIFO_create();
        s->ready_queue_insert_fn = fifo_create;
        s->ready_queue_remove_fn = fifo_remove;
        s->ready_queue_size_fn = fifo_size;
        s->ready_queue_destroy_fn = fifo_destroy;
    }
    else if(type == ROUND_ROBIN) {
        s->ready_queue = RoundRobin_create();
        s->ready_queue_insert_fn = round_robin_push;
        s->ready_queue_remove_fn = round_robin_remove;
        s->ready_queue_size_fn = round_robin_size;
        s->ready_queue_destroy_fn = round_robin_destroy;
    }
    else if(type == PRIORITY) {
        s->ready_queue = priority_create();
        s->ready_queue_insert_fn = priority_push;
        s->ready_queue_remove_fn = priority_remove;
        s->ready_queue_size_fn = priority_size;
        s->ready_queue_destroy_fn = priority_destroy;
    }
    s->finish = 0;
    s->type = type;

    // Inicia o mutex e variável de condição do escalonador
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);

    return s;
}

void scheduler_destroy(Scheduler * s) {

    if (s->log != NULL) {
        fprintf(s->log, "Escalonador terminou execução de todos processos\n");
        fclose(s->log);
    }

    s->ready_queue_destroy_fn(s->ready_queue);
    pthread_mutex_destroy(&s->lock);
    pthread_cond_destroy(&s->cond);
    free(s);
}