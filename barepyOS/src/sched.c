#include "sched.h"
#include "kheap.h"

struct pcb_s *current_process;
struct pcb_s kmain_process;
struct pcb_s *terminated_process;
void insert_terminated_pcb(struct pcb_s* current_process);

void
sched_init(void)
{
    terminated_process=0;
    current_process = &kmain_process;
    kmain_process.p_pcb=&kmain_process;
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
     
     if(kmain_process.n_pcb==0) //on insere un processus si il est le premier
     {
        pcb->n_pcb=pcb;
        pcb->p_pcb=pcb;
        kmain_process.n_pcb=pcb;
     }
     else //on insere un processus si il existe d'autres processus
     {
        pcb->n_pcb=kmain_process.n_pcb;
        pcb->p_pcb=pcb->n_pcb->p_pcb;
        pcb->n_pcb->p_pcb=pcb;
        pcb->p_pcb->n_pcb=pcb;
        kmain_process.n_pcb=pcb;
     }

     pcb->state = RUNNING;
     pcb->errorCode = 0;
     pcb->entry=entry;
     return pcb;
}

void
elect(void)
{  
    if(current_process->state==TERMINATED)
    {
        struct pcb_s* previous_pcb = current_process->p_pcb;
        insert_terminated_pcb(current_process);
        if(previous_pcb->state == TERMINATED) //Si il ne restait qu'un pcb
        {
            current_process=&kmain_process;
            kmain_process.n_pcb=0;
        }
        else current_process=previous_pcb->n_pcb;
    }
    else if(current_process->n_pcb != 0)
    {
        current_process = current_process->n_pcb;
    }
    else
    {
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

void
insert_terminated_pcb(struct pcb_s* current_pcb)
{
    //on enleve le current_pcb de sa file
    current_pcb->n_pcb->p_pcb=current_pcb->p_pcb;
    current_pcb->p_pcb->n_pcb=current_pcb->n_pcb;
        
    //on ajoute le processus aux processus termines
    if(terminated_process==0)
    {
        terminated_process=current_pcb;
        terminated_process->p_pcb=terminated_process;
        terminated_process->n_pcb=terminated_process;
    }
    else 
    {
        current_pcb->n_pcb=terminated_process;
        current_pcb->p_pcb=terminated_process->p_pcb;
        current_pcb->p_pcb->n_pcb=current_pcb;
        terminated_process->p_pcb=current_pcb;
    }
}

/*
    if(current_process->state==GARBAGE){
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
*/
