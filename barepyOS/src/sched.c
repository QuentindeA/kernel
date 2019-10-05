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
     pcb->sp = (int*)(kAlloc(10000)+10000-1);
     pcb->lr = (int*)entry;
     __asm("mrs  r0, cpsr");
     __asm("mov %0, r0" : "=r" (pcb->cpsr));
     return pcb;
}
