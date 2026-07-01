#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "../control_structures/pcb.h"
#include "../data_structures/vector.h"

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
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    fscanf(file, "%d", &amount_of_processes);
    tVector* processes = vector_create(amount_of_processes);

    for (int i = 0; i < amount_of_processes; i++){
        fscanf(file, "%d", &process_duration);
        fscanf(file, "%d", &priority);
        fscanf(file, "%d", &amount_of_threads);
        fscanf(file, "%d", &t0);

        vector_push(processes,
            pcb_create(i, process_duration, priority, amount_of_threads, t0));
    }
    
    vector_sort(processes, pcb_compare);
    fscanf(file, "%d", &scheduler_type);

    // Selecionar o scheduler

    fclose(file);
    vector_destroy(processes, pcb_destroy);

    return 0;
}
