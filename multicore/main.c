#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "../control_structures/pcb.h"
#include "../control_structures/tcb.h"
#include "../data_structures/vector.h"
#include "../schedulers/scheduler.h"

#define CORES 1

int main(int argc, char const *argv[])
{
    int amount_of_processes;
    int process_duration;
    int priority;
    int amount_of_threads;
    int t0;
    int scheduler_type;

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        return 1;
    }

    fscanf(file, "%d", &amount_of_processes);
    tVector* processes = vector_create(amount_of_processes);

    for (int i = 0; i < amount_of_processes; i++){
        fscanf(file, "%d", &process_duration);
        fscanf(file, "%d", &priority);
        fscanf(file, "%d", &amount_of_threads);
        fscanf(file, "%d", &t0);

        PCB *pcb = pcb_create(i + 1, process_duration, priority, amount_of_threads, t0);
        vector_push(processes, pcb);
    }

    vector_sort(processes, pcb_compare);
    fscanf(file, "%d", &scheduler_type);
    
    FILE *log = fopen("log_execucao_minikernel.txt", "w");
    Scheduler * scheduler = scheduler_create(scheduler_type, log);
    
    AuxScheduler * aux_core1 = aux_scheduler_create(scheduler, 0, MULTI);
    AuxScheduler * aux_core2 = aux_scheduler_create(scheduler, 1, MULTI);

    pthread_t scheduler_thread1;
    pthread_t scheduler_thread2;
    
    pthread_create(&scheduler_thread1, NULL, scheduler_run, aux_core1);
    pthread_create(&scheduler_thread2, NULL, scheduler_run, aux_core2);

    int time_1 = 0;
    int time_2 = 0;
    
    for (int i = 0; i < amount_of_processes; i++)
    {
        PCB *pcb = vector_get(processes, i);
        time_2 = get_start_time(pcb);
        usleep((time_2 - time_1) * 1000);
        pcb_start_threads(pcb);
        scheduler_push(scheduler, pcb);
        time_1 = time_2;
    }
    
    scheduler_finish(scheduler, 2);
    
    pthread_join(scheduler_thread1, NULL);
    pthread_join(scheduler_thread2, NULL);

    fclose(file);
    vector_destroy(processes, pcb_destroy);
    scheduler_destroy(scheduler);

    return 0;
}