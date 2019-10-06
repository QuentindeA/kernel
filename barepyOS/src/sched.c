#include "sched.h"
#include "kheap.h"

struct pcb_s *current_process;
struct pcb_s kmain_process;

void
sched_init(void)
{
    current_process = &kmain_process;
    kheap_init();
}

struct pcb_s*
create_process(func_t entry)
{
     struct pcb_s* pcb = (struct pcb_s*)(kAlloc(sizeof(struct pcb_s)));
     pcb->pAlloc = (int*)(kAlloc(10000));
     pcb->sp = (int*)(pcb->pAlloc+10000-1);
     pcb->lr = (int*)start_current_process;
     __asm("mrs  r0, cpsr");
     __asm("mov %0, r0" : "=r" (pcb->cpsr));
     pcb->n_pcb = 0;
     pcb->p_pcb = &kmain_process;
     while(pcb->p_pcb->n_pcb!=0)
        pcb->p_pcb = pcb->p_pcb->n_pcb;
     pcb->p_pcb->n_pcb = pcb;
     pcb->state = RUNNING;
     pcb->errorCode = 1;
     pcb->entry=entry;
     return pcb;
}

void
elect(void)
{
    if(current_process->state==TERMINATED){
        struct pcb_s* terminated_pcb = current_process;
        kFree((void*)terminated_pcb->pAlloc, 10000);
        current_process = terminated_pcb->p_pcb;
        if(terminated_pcb->n_pcb!=0) {
            current_process->n_pcb = terminated_pcb->n_pcb;
            current_process->n_pcb->p_pcb = current_process;
        } else 
            current_process->n_pcb = 0;
        kFree((void*)terminated_pcb, sizeof(struct pcb_s));
    }
    if(current_process->n_pcb!=0)
        current_process = current_process->n_pcb;
    else if(kmain_process.n_pcb!=0)
        current_process = kmain_process.n_pcb;
    else {
        current_process = &kmain_process;
        terminate_kernel();
    }
}

void
start_current_process(void)
{
    int errorCode;
    errorCode = current_process->entry();
    sys_exit(errorCode);
}
