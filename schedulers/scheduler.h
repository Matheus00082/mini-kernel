#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include "../control_structures/pcb.h"

typedef struct Scheduler Scheduler;

typedef enum {
    SINGLE,
    MULTI
} AuxSchedulerType;

typedef struct AuxScheduler
{
    Scheduler *s;
    int ID;
    AuxSchedulerType type;
} AuxScheduler;

AuxScheduler *aux_scheduler_create(Scheduler *s, int id, AuxSchedulerType type);
void aux_scheduler_destroy(AuxScheduler *);

Scheduler * scheduler_create(SchedulerType type, FILE * log);

/**
 * @brief Rotina de execução das threads do escalonador (os núcleos).
 *
 * CPU busca novos processos na fila de prontos e gerencia suas fatias de tempo,
 * registrando logs no arquivo texto. Implementando a lógica
 * específica para lidar com cada scheduler.
 *
 * @param a Ponteiro void que será convertido para um AuxScheduler com o contexto do núcleo.
 * @return Sempre retorna NULL ao terminar.
 */
void *scheduler_run(void *);

/**
 * @brief Adiciona um novo bloco de processo à fila de prontos.
 * @param s Ponteiro para o escalonador principal.
 * @param p Ponteiro para o PCB do processo a ser enfileirado.
 */
void scheduler_push(Scheduler *s, PCB *p);

/**
 * @brief Sinaliza ao escalonador que nenhum processo novo será adicionado à fila.
 * @param s Ponteiro para o escalonador principal.
 * @param cores A quantidade de núcleos utilizados pela simulação.
 */
void scheduler_finish(Scheduler *s, int cores);
void scheduler_destroy(Scheduler * s);

#endif