#ifndef _SCHED_H_
#define _SCHED_H_

#define RUNNING 0
#define TERMINATED 1
#define GARBAGE 2

typedef int (func_t) (void);

struct pcb_s{
    int rx[13];
    int* sp;
    int* lr;
    int cpsr;
    struct pcb_s* n_pcb; //pcb suivant
    struct pcb_s* p_pcb; //pcb_precedant
    int state;
    int* pAlloc;
    int errorCode;
    func_t* entry;
    int *lr_irq;
};

#include "kheap.h"
#include "syscall.h"

void sched_init(void);
struct pcb_s* create_process(func_t entry);	
void elect(void);
void start_current_process(void);

#endif
