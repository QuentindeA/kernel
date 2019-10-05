#include "syscall.h"
#include "sched.h"

struct pcb_s pcb1, pcb2;
struct pcb_s *p1, *p2;

void user_process_1();
void user_process_2();

void
kmain(void)
{
    sched_init();
    
    p1=create_process((func_t*) &user_process_1);
    p2=create_process((func_t*) &user_process_2);

    __asm("cps 0x10");
    //**************************************************
    //Userland
    
    sys_yieldto(p1);
    
    PANIC();
}


void user_process_1()
{
    int v1=5;
    while(1)
    {
        v1++;
        sys_yieldto(p2);
    }
}

void user_process_2()
{
    int v2=-12;
    while(1)
    {
        v2-=2;
        sys_yieldto(p1);
    }
}
