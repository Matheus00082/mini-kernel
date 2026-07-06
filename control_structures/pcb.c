#include "pcb.h"
#include "tcb.h"
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
    int quantum_left;      // bursts liberados pelo escalonador e ainda não consumidos
    int bursts_done;       // bursts concluídos e ainda não consumidos pelo escalonador
    int working_threads;   // threads que ainda possuem carga de trabalho
    int last_burst;        // duração do último burst executado (p/ segurar o quantum)

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
    p->bursts_done = 0;
    p->working_threads = num_threads;
    p->last_burst = 0;
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

    pthread_cond_init(&p->cv_2, NULL);
    p->thread_ids = calloc(num_threads, sizeof(pthread_t));

    return p;
}

void pcb_destroy(void *pcb)
{
    PCB *p = (PCB *)pcb;
    free(p->thread_ids);
    pthread_cond_destroy(&p->cv_1);
    pthread_cond_destroy(&p->cv_2);
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
    if (p->remaining_time > 0 && p->state != FINISHED) {
        p->quantum_left++; // libera UM burst para UMA thread executar
        p->state = RUNNING;
        pthread_cond_broadcast(&p->cv_1);
    }
    pthread_mutex_unlock(&p->mutex_1);
}

int pcb_finish(PCB *p)
{
    pthread_mutex_lock(&p->mutex_1);

    // O escalonador dorme até que uma thread termine seu burst ou o processo acabe
    while (p->bursts_done == 0 && p->state != FINISHED) {
        pthread_cond_wait(&p->cv_2, &p->mutex_1);
    }

    if (p->bursts_done > 0) {
        p->bursts_done--; // consome o burst concluído
    }

    int is_finished = (p->remaining_time <= 0);
    pthread_mutex_unlock(&p->mutex_1);
    return is_finished;
}

void pcb_start_threads(PCB *p) {
    for (int i = 0; i < p->num_threads; i++) {
        TCB *tcb = tcb_create(p, i);
        pthread_create(&p->thread_ids[i], NULL, thread_routine, tcb);
    }
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

int get_working_threads(PCB *p)
{
    pthread_mutex_lock(&p->mutex_1);
    int n = p->working_threads;
    pthread_mutex_unlock(&p->mutex_1);
    return n;
}

int get_last_burst(PCB *p)
{
    pthread_mutex_lock(&p->mutex_1);
    int t = p->last_burst;
    pthread_mutex_unlock(&p->mutex_1);
    return t;
}

void threads_join(PCB *p)
{
    for (int i = 0; i < p->num_threads; i++)
    {
        pthread_join(p->thread_ids[i], NULL);
    }
    
}

void* thread_routine(void* arg) {
    TCB* tcb = (TCB*)arg;
    PCB* pcb = get_pcb(tcb);
    int workload = pcb->process_len / pcb->num_threads;

    pthread_mutex_lock(&pcb->mutex_1);
    while (workload > 0 && pcb->state != FINISHED) {

        // Bloqueia até o escalonador liberar um burst (evita execução sem posse de CPU)
        while (!(pcb->state == RUNNING && pcb->quantum_left > 0) && pcb->state != FINISHED) {
            pthread_cond_wait(&pcb->cv_1, &pcb->mutex_1);
        }

        if (pcb->state == FINISHED) {
            break;
        }

        pcb->quantum_left--; // consome a vaga de burst liberada para esta thread
        int time_spent = (workload > QUANTUM_TIME) ? QUANTUM_TIME : workload;
        pthread_mutex_unlock(&pcb->mutex_1);

        usleep(time_spent * 1000); // simula de fato a execução da thread

        pthread_mutex_lock(&pcb->mutex_1);
        workload -= time_spent;
        pcb->remaining_time -= time_spent;
        pcb->last_burst = time_spent;

        if (workload <= 0) {
            pcb->working_threads--; // esta thread não possui mais carga
        }

        // Correção de resto de divisão matemática
        if (workload <= 0 && pcb->remaining_time > 0 && pcb->remaining_time < pcb->num_threads) {
            pcb->remaining_time = 0;
        }

        // Se o processo acabou
        if (pcb->remaining_time <= 0 && pcb->state != FINISHED) {
            pcb->state = FINISHED;
            pthread_cond_broadcast(&pcb->cv_1); // Acorda as outras threads
        }

        // Registra que ESTA thread concluiu seu burst e avisa o escalonador
        pcb->bursts_done++;
        pthread_cond_broadcast(&pcb->cv_2);
    }
    pthread_mutex_unlock(&pcb->mutex_1);

    free(tcb);
    return NULL;
}