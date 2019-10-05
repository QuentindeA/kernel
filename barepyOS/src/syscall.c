#include "syscall.h"
#include "util.h"
#include "hw.h"
#include "sched.h"
#include <stdint.h>

#define PANIC() do { kernel_panic(__FILE__,__LINE__) ; } while(0)
#define ASSERT(exp) do { if(!(exp)) PANIC(); } while(0)

int unsigned* sp_svc;
extern struct pcb_s *current_process;
//struct pcb_s *current_process;

void __attribute__((naked))
C_swi_handler(void)
{
    int N;

    __asm("stmfd sp!,{r0-r12,lr}");
    __asm("mov %0, r0" : "=r" (N));
    __asm("mov %0, sp" : "=r" (sp_svc));
    
    switch(N)
    {
        case 1:
            do_sys_reboot();
            break;
        case 2:
            do_sys_nop();
            break;
        case 3:
            do_sys_settime();
            break;
        case 4:
            do_sys_gettime();
            break;
        case 5:
            do_sys_yieldto();
            break;
        default:
            PANIC();
            break;
    }

    __asm("ldmfd sp!,{r0-r12,pc}^");
}

void
sys_reboot(void)
{
    __asm("mov r0, #1");
    __asm("swi #0");
}

void
do_sys_reboot(void)
{
    __asm("b 0x8000");
}

void
sys_nop(void)
{
    __asm("mov r0, #2");
    __asm("swi #0");
}

void
do_sys_nop(void)
{
}

void 
sys_settime(uint64_t date_ms)
{
    __asm("mov r0, #3");
    __asm("mov r2, %0" : : "r" (date_ms));
    __asm("swi #0");
}

void 
do_sys_settime(void)
{
    uint64_t date_ms;
    date_ms = *(uint64_t*)(sp_svc+2);
    set_date_ms(date_ms);
}

uint64_t
sys_gettime(void)
{
    uint64_t date;
    __asm("mov r0, #4");
    __asm("swi #0");
    __asm("mov %0, r2" : "=r" (date));
    return date;
}

void
do_sys_gettime(void)
{
    uint64_t date;
    
    date = get_date_ms();
    *(sp_svc+2) = (int unsigned)(date & 0xFFFFFFFF);
    *(sp_svc+3) = (int unsigned)(date >> 32);
    
}

void
sys_yieldto(struct pcb_s* dest)
{
    __asm("mov r0, #5");
    __asm("mov r1, %0" :: "r" (dest) : "r0");
    __asm("swi #0");
}


void
do_sys_yieldto()
{
    struct pcb_s* dest = (void*)*(sp_svc+1);
    for(int i=0; i<13; i++)
        current_process->rx[i] = *(sp_svc+i);
    
    __asm("cps 0X1F");
    __asm("mov %0, lr" : "=r" (current_process->lr));
    __asm("mov %0, sp" : "=r" (current_process->sp));
    __asm("cps 0x13");
    
    __asm("mrs r0, spsr");
    __asm("mov %0, r0" : "=r" (current_process->cpsr));
    
    current_process = dest;
    
    for(int i=0; i<13; i++)
        *(sp_svc+i) = current_process->rx[i];
    
    __asm("cps 0x1F");
    __asm("mov lr, %0" :: "r" (current_process->lr));
    __asm("mov sp, %0" :: "r" (current_process->sp));
    __asm("cps 0x13");
    
    __asm("mov r0, %0" :: "r" (current_process->cpsr));
    __asm("msr spsr, r0");
}


