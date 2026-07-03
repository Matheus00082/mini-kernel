#include "tcb.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pcb.h"
#include "tcb.h"

struct TCB{
    PCB* pcb;
    int thread_index;
};
