#ifndef _SCHED_H_
#define _SCHED_H_

struct pcb_s{
    int rx[13];
    int* sp;
    int* lr;
};

void sched_init(void);

#endif
