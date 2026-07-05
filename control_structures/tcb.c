#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pcb.h"
#include "tcb.h"
#include <pthread.h>

struct TCB{
    PCB* pcb;
    int thread_index;
};


TCB* tcb_create(PCB* pcb, int thread_index) {
    TCB* tcb = malloc(sizeof(TCB));
    tcb->pcb = pcb;
    tcb->thread_index = thread_index;
    return tcb;
}

TCB* destroy(TCB* tcb) {
    free(tcb);
}

PCB* get_pcb(TCB* tcb){
    return tcb->pcb;
}