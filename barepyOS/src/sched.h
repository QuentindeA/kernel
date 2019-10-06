#ifndef _SCHED_H_
#define _SCHED_H_

#define RUNNING 0
#define TERMINATED 1

typedef int (func_t) (void);

struct pcb_s{
    int rx[13];
    int* sp;
    int* lr;
    int cpsr;
    struct pcb_s* n_pcb;
    struct pcb_s* p_pcb;
    int state;
    int* pAlloc;
    int errorCode;
    func_t* entry;
};

#include "kheap.h"
#include "syscall.h"

void sched_init(void);
struct pcb_s* create_process(func_t entry);	
void elect(void);
void start_current_process(void);

#endif
