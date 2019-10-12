#include "util.h"
#include "syscall.h"
#include "sched.h"
#include "hw.h"

int user_process_1()
{
    int v1=5;
    while(1)
    {
        v1++;
    }
}
int user_process_2()
{
    int v2=-12;
    while(1)
    {
        v2-=2;
    }
}
int user_process_3()
{
    int v3=0;
    while(1)
    {
        v3+=5;
    }
}

void kmain( void )
{
    vmem_init();
    sched_init();
    
    create_process(&user_process_1);
    create_process(&user_process_2);
    create_process(&user_process_3);
    
    __asm("cps 0x10"); // switch CPU to USER mode
    // **********************************************************************
    
    sys_nop();
    
    while(1)
    {
        sys_yield();
    }
}
