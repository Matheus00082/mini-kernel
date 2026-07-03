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
    int state;
    int quantum;
    FILE * log;
    pthread_mutex_t lock;
    pthread_cond_t cond;

    func_queue_insert queue_insert;
    func_queue_remove queue_remove;
    func_queue_size queue_size;
    func_queue_destroy queue_destroy;
};

AuxScheduler *aux_scheduler_create(Scheduler *s, int core_quantity){
    AuxScheduler *aux = calloc(1, sizeof(AuxScheduler));
    aux->s = s;
    aux->core_quantity = core_quantity;
    return aux;
}

void aux_scheduler_destroy(AuxScheduler *aux){
    free(aux);
}

Scheduler * scheduler_create(SchedulerType type, FILE * log) {
    Scheduler * s = (Scheduler*)calloc(1, sizeof(Scheduler));

    s->quantum = QUANTUM_TIME;
    s->log = log;

    // Define as funções da fila de prontos a depender do algoritmo
    if(type == FCFS) {
        s->ready_queue = fifo_create(pcb_destroy);
        s->queue_insert = fifo_push;
        s->queue_remove = fifo_remove;
        s->queue_size = fifo_size;
        s->queue_destroy = fifo_destroy;
    }
    else if(type == ROUND_ROBIN) {
        s->ready_queue = round_robin_create(pcb_destroy);
        s->queue_insert = round_robin_push;
        s->queue_remove = round_robin_remove;
        s->queue_size = round_robin_size;
        s->queue_destroy = round_robin_destroy;
    }
    else if(type == PRIORITY) {
        s->ready_queue = priority_create(pcb_destroy);
        s->queue_insert = priority_push;
        s->queue_remove = priority_remove;
        s->queue_size = priority_size;
        s->queue_destroy = priority_destroy;
    }

    s->state = UNCONCLUDED;
    s->type = type;

    // Inicia o mutex e variável de condição do escalonador
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);

    return s;
}

void *scheduler_run(void *a){
    AuxScheduler *aux = (AuxScheduler *)a;
    Scheduler *s = aux->s;
    int core_quantity = aux->core_quantity;
    int old_pid = -1;

    while(1){
        pthread_mutex_lock(&s->lock);

        //Impede CPU ociosa (ainda há processos para entrar)
        while (s->queue_size(s->ready_queue) == 0 && s->state == UNCONCLUDED)
        {
            pthread_cond_wait(&s->cond, &s->lock);
        }
        
        //Todos os processos finalizados
        if(s->state == CONCLUDED && s->queue_size(s->ready_queue) == 0){
            pthread_mutex_unlock(&s->lock);
            aux_scheduler_destroy(aux);
            break;
        }

        PCB *pcb = s->queue_remove(s->ready_queue);
        pthread_mutex_unlock(&s->lock);

        const char *type = "EMPTY";

        if(s->type == FCFS){type = "FCFS";}
        else if(s->type == ROUND_ROBIN){type = "RR";}
        else{type = "PRIORITY";}

        if(s->type == FCFS) {
            if (core_quantity > 0)
                fprintf(s->log, "[%s] Executando processo PID %d // processador %d\n", type, get_pid(pcb), core_quantity);
            else
                fprintf(s->log, "[%s] Executando processo PID %d\n", type, get_pid(pcb));
        }
        else if(s->type == ROUND_ROBIN) {
            if (core_quantity > 0)
                fprintf(s->log, "[%s] Executando processo PID %d com quantum %dms // processador %d\n", type, get_pid(pcb), QUANTUM_TIME, core_quantity);
            else
                fprintf(s->log, "[%s] Executando processo PID %d com quantum %dms\n", type, get_pid(pcb), QUANTUM_TIME);
        }
        else if(s->type == PRIORITY){
            if (old_pid != get_pid(pcb)) {
                if (core_quantity > 0)
                    fprintf(s->log, "[%s] Executando processo PID %d prioridade %d // processador %d\n", type, get_pid(pcb), get_priority(pcb), core_quantity);
                else
                    fprintf(s->log, "[%s] Executando processo PID %d prioridade %d\n", type, get_pid(pcb), get_priority(pcb));
            }
        }

        pcb_running(pcb);

        if(s->type == FCFS) {
            threads_join(pcb);
            fprintf(s->log, "[%s] Processo PID %d finalizado\n", type, get_pid(pcb));
        }
        else if(s->type == ROUND_ROBIN || s->type == PRIORITY) {
            usleep(s->quantum * 1000);
            
            if(pcb_finish(pcb)) {
                threads_join(pcb);
                fprintf(s->log, "[%s] Processo PID %d finalizado\n", type, get_pid(pcb));
            }
            else {
                pcb_ready(pcb);
                s->queue_insert(s->ready_queue, pcb);
            }
        }

        old_pid = get_pid(pcb);
    }

    return NULL;
}

void *scheduler_push(Scheduler *s, PCB *p)
{
    pthread_mutex_lock(&s->lock);
    s->queue_insert(s->ready_queue, p);
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void scheduler_finish(Scheduler *s)
{
    pthread_mutex_lock(&s->lock);
    s->state = CONCLUDED;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void scheduler_destroy(Scheduler * s) {

    if (s->log != NULL) {
        fprintf(s->log, "Escalonador terminou execução de todos processos\n");
        fclose(s->log);
    }

    s->queue_destroy(s->ready_queue);
    pthread_mutex_destroy(&s->lock);
    pthread_cond_destroy(&s->cond);
    free(s);
}