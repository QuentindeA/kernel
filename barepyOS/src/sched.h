#ifndef _SCHED_H_
#define _SCHED_H_

#include "kheap.h"

struct pcb_s{
    int rx[13];
    int* sp;
    int* lr;
    int cpsr;
};

typedef int (func_t) (void);

void sched_init(void);
struct pcb_s* create_process(func_t entry);	

#endif
