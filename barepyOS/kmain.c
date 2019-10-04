#include "syscall.h"
//#include "sched.h"

struct pcb_s pcb1, pcb2;
struct pcb_s *p1, *p2;

void
kmain(void)
{
    __asm("cps 0x10");
    //**************************************************
    //Userland
    
    //sched_init();
    
    p1=&pcb1;
    p2=&pcb2;
    
    sys_yieldto(p1);

    sys_reboot();
}
