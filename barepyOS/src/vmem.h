#ifndef VMEM_H
#define VMEM_H

#define PAGE_SIZE 4096*4 //in bytes
#define SECON_LVL_TT_SIZE 256*4 //in bytes
#define FIRST_LVL_TT_SIZE 4096*4 //in bytes

void vmem_init(void);
int init_kern_translation_table(void);
void start_mmu_C(void);
void configure_mmu_C(void);

#endif
