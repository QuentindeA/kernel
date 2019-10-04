#include "syscall.h"
#include "util.h"
#include "hw.h"
#include <stdint.h>

#define PANIC() do { kernel_panic(__FILE__,__LINE__) ; } while(0)
//#define ASSERT(exp) do { if(!(exp)) PANIC(); } while(0)

struct pcb_s{

}pcb_s


void __attribute__((naked))
C_swi_handler(void)
{
    int N;

    __asm("stmfd sp!,{r0-r12,lr}^");
    
    __asm("mov %0, r0" : "=r" (N));
    

    switch(N)
    {
        case 1:
            do_sys_reboot();
            break;
        case 2:
            do_sys_nop();
            break;
        case 3:
            __asm("mov r0, sp");
            do_sys_settime();
            break;
        case 4:
            __asm("mov r0, sp");
            do_sys_gettime();
            break;
        case 5:
            __asm("mov r0, sp");
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
    __asm("ldrd r2, [r0, #8]");
    __asm("mov r2, %0" : "=r" (date_ms));
    set_date_ms(date_ms);
}

uint64_t
sys_gettime(void)
{
    uint64_t date;
    int lr;
    __asm("mov %0, lr" : "=r" (lr));
    __asm("mov r0, #4");
    __asm("add r1, pc, #4");
    __asm("mov lr, r1");
    __asm("swi #0");
    
    __asm("mov %0, r2" : "=r" (date));
    
    __asm("mov lr, %0" :: "r" (lr));
    
    return date;
}

void
do_sys_gettime(void)
{
    uint64_t date;
    int sp;
    
    __asm("mov %0, r0" : "=r" (sp));
    
    date = get_date_ms();
    
    __asm("mov r2, %0" :: "r" (date));
    __asm("mov r1, %0" :: "r" (sp) : "r2", "r3");
    __asm("strd r2, [r1, #8]");
}

void
sys_yieldto(struct pcb_s* dest)
{
    __asm("mov r0, #5");
    __asm("mov r1, %0" :: "r" (pcb_s) : "r0");
    __asm("swi #5");
}

void
do_sys_yieldto()
{
    struct pcb_s* dest;
    __asm("ldr r1, [r0, #4]");
    __asm("mov %0, r1" : "=r" (pcb_s));
}

