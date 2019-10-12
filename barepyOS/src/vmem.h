#ifndef VMEM_H
#define VMEM_H

#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE SECON_LVL_TT_COUN*4 //in bytes
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE FIRST_LVL_TT_COUN*4 //in bytes
#define PAGE_SIZE 4096*4 //in bytes

#include "sched.h"

extern uint32_t __kernel_heap_end__;

void vmem_init(void);
unsigned int init_kern_translation_table(void);
void start_mmu_C(void);
void configure_mmu_C(void);
uint32_t vmem_translate(uint32_t va, struct pcb_s* process);

#endif
