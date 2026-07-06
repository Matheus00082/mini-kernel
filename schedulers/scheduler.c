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

    int active_processes;
    PCB * current[2];

    func_queue_insert queue_insert;
    func_queue_remove queue_remove;
    func_queue_size queue_size;
    func_queue_destroy queue_destroy;
}; // controlador de processos

static PCB * scheduler_join_target(Scheduler *s, AuxSchedulerType core_type)
{
    if (core_type != MULTI || s->type != FCFS) {
        return NULL;
    }

    for (int i = 0; i < 2; i++) {
        PCB *p = s->current[i];
        if (p == NULL) {
            continue;
        }
        int serving = (s->current[0] == p) + (s->current[1] == p);
        if (get_working_threads(p) > serving) {
            return p;
        }
    }
    return NULL;
}

AuxScheduler *aux_scheduler_create(Scheduler *s, int id, AuxSchedulerType type){
    AuxScheduler *aux = calloc(1, sizeof(AuxScheduler));
    aux->s = s;
    aux->ID = id;
    aux->type = type;
    return aux;
}

void aux_scheduler_destroy(AuxScheduler *aux){
    free(aux);
}

Scheduler * scheduler_create(SchedulerType type, FILE * log) {
    Scheduler * s = (Scheduler*)calloc(1, sizeof(Scheduler));

    s->quantum = QUANTUM_TIME;
    s->log = log;
    s->active_processes = 0;

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

    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);

    return s;
}

void *scheduler_run(void *a){
    AuxScheduler *aux = (AuxScheduler *)a;
    Scheduler *s = aux->s;
    AuxSchedulerType core_type = aux->type;
    int core_id = aux->ID;
    int old_pid = -1;

    while(1){
        pthread_mutex_lock(&s->lock);

        while (s->queue_size(s->ready_queue) == 0 && s->state == UNCONCLUDED
            && scheduler_join_target(s, core_type) == NULL)
        {
            pthread_cond_wait(&s->cond, &s->lock);
        }

        if(s->state == CONCLUDED && s->queue_size(s->ready_queue) == 0 && s->active_processes == 0){
            pthread_mutex_unlock(&s->lock);
            aux_scheduler_destroy(aux);
            break;
        }

        PCB *pcb = NULL;
        int joined = 0;

        if (s->queue_size(s->ready_queue) > 0) {
            pcb = s->queue_remove(s->ready_queue);
        }
        else {
            pcb = scheduler_join_target(s, core_type);
            if (pcb == NULL) {
                pthread_cond_wait(&s->cond, &s->lock);
                pthread_mutex_unlock(&s->lock);
                continue;
            }
            joined = 1;
        }

        s->current[core_id] = pcb;
        s->active_processes++;

        const char *type = "EMPTY";

        if(s->type == FCFS){type = "FCFS";}
        else if(s->type == ROUND_ROBIN){type = "RR";}
        else{type = "PRIORITY";}

        if(s->type == FCFS) {
            if (core_type == MULTI)
                fprintf(s->log, "[%s] Executando processo PID %d // processador %d\n", type, get_pid(pcb), core_id);
            else
                fprintf(s->log, "[%s] Executando processo PID %d\n", type, get_pid(pcb));
        }
        else if(s->type == ROUND_ROBIN) {
            if (core_type == MULTI)
                fprintf(s->log, "[%s] Executando processo PID %d com quantum %dms // processador %d\n", type, get_pid(pcb), QUANTUM_TIME, core_id);
            else
                fprintf(s->log, "[%s] Executando processo PID %d com quantum %dms\n", type, get_pid(pcb), QUANTUM_TIME);
        }
        else if(s->type == PRIORITY){
            if (old_pid != get_pid(pcb)) {
                if (core_type == MULTI)
                    fprintf(s->log, "[%s] Executando processo PID %d prioridade %d // processador %d\n", type, get_pid(pcb), get_priority(pcb), core_id);
                else
                    fprintf(s->log, "[%s] Executando processo PID %d prioridade %d \n", type, get_pid(pcb), get_priority(pcb));
            }
        }

        old_pid = get_pid(pcb);

        if (core_type == MULTI && s->type == FCFS) {
            pthread_cond_broadcast(&s->cond);
        }

        pthread_mutex_unlock(&s->lock);

        if(s->type == FCFS) {
            pcb_running(pcb);
            while (!pcb_finish(pcb)) {
                pcb_running(pcb);
            }

            if (!joined) {
                threads_join(pcb);

                pthread_mutex_lock(&s->lock);

                fprintf(s->log, "[%s] Processo PID %d finalizado\n", type, get_pid(pcb));

                if (core_type == MULTI) {
                    int other = 1 - core_id;
                    if (s->current[other] != NULL && s->current[other] != pcb) {
                        fprintf(s->log, "[%s] Executando processo PID %d // processador %d\n", type, get_pid(s->current[other]), other);
                    }
                }
            }
            else {
                pthread_mutex_lock(&s->lock);

                while (s->current[1 - core_id] == pcb) {
                    pthread_cond_wait(&s->cond, &s->lock);
                }
            }

            // Registra o PCB no vetor current indicando que ele tomou posse do núcleo atual
            s->current[core_id] = NULL;
            s->active_processes--;

            if (core_type == MULTI) {
                pthread_cond_broadcast(&s->cond);
            }

            if(s->state == CONCLUDED && s->queue_size(s->ready_queue) == 0 && s->active_processes == 0) {
                pthread_cond_broadcast(&s->cond);
            }
            pthread_mutex_unlock(&s->lock);
        }
        else if(s->type == ROUND_ROBIN || s->type == PRIORITY) {
            pcb_running(pcb);
            int finished = pcb_finish(pcb);

            int spent = get_last_burst(pcb);
            if (spent < QUANTUM_TIME) {
                usleep((QUANTUM_TIME - spent) * 1000);
            }

            if(finished) {
                threads_join(pcb);

                pthread_mutex_lock(&s->lock);
                fprintf(s->log, "[%s] Processo PID %d finalizado\n", type, get_pid(pcb));
                s->current[core_id] = NULL;
                s->active_processes--;

                if(s->state == CONCLUDED && s->queue_size(s->ready_queue) == 0 && s->active_processes == 0) {
                    pthread_cond_broadcast(&s->cond);
                }
                pthread_mutex_unlock(&s->lock);
            }
            else {
                pcb_ready(pcb);
                pthread_mutex_lock(&s->lock);
                s->current[core_id] = NULL;
                s->queue_insert(s->ready_queue, pcb);
                s->active_processes--;
                pthread_cond_signal(&s->cond);
                pthread_mutex_unlock(&s->lock);
            }
        }

    }

    return NULL;
}

void scheduler_push(Scheduler *s, PCB *p)
{
    pthread_mutex_lock(&s->lock);
    s->queue_insert(s->ready_queue, p);
    // Acorda thread que pode estar dormindo esperando por tarefas
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void scheduler_finish(Scheduler *s, int cores)
{
    pthread_mutex_lock(&s->lock);
    s->state = CONCLUDED; // impede inserção de novos dados

    if(cores ==1 )
        pthread_cond_signal(&s->cond);
    else
        pthread_cond_broadcast(&s->cond);

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