#ifndef TCB_H
#define TCB_H

typedef struct TCB TCB;

TCB* tcb_create(PCB *p, int i);
void destroy(TCB* tcb);
PCB* get_pcb(TCB* tcb);
#endif