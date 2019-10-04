#include "syscall.h"

void
kmain(void)
{
    __asm("cps 0x10");
    //**************************************************
    //Userland
    
    uint64_t date = sys_gettime();
    date++;

    sys_reboot();
}
