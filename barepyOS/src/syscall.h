#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "syscall.h"
#include "util.h"
#include "hw.h"
#include "sched.h"
#include <stdint.h>

void __attribute__((naked)) C_swi_handler(void);
void sys_reboot(void);
void do_sys_reboot(void);
void sys_nop(void);
void do_sys_nop(void);
void sys_settime(uint64_t date_ms);
void do_sys_settime(void);
uint64_t sys_gettime(void);
void do_sys_gettime(void);
void sys_yieldto(struct pcb_s* dest);
void do_sys_yieldto(void);
void sys_yield(void);
void do_sys_yield(void);
void sys_exit(int status);
void do_sys_exit(void);

#endif
