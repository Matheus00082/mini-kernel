#include "tcb.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pcb.h"
#include "tcb.h"

typedef struct {
    PCB* pcb;
    int thread_index;
} TCB;
