#include "kheap.h"
#include "vmem.h"
#include "hw.h"

void vmem_init(void)
{
    //init physical memory
    kheap_init();
    
        //Configure coprocessor registers.
    configure_mmu_C();
    __asm("mcr p15, 0, r0, c7, c7, 0");
        
        //activate interrupts
    ENABLE_IRQ();
        //3. De-activate and invalidate the instruction cache.
        //Then, it is possible to re-activate it when activating the MMU.
        // Here is the code for this purpose : mcr p15, 0, r0, c7, c7, 0
    start_mmu_C();
}

unsigned int
init_kern_translation_table(void)
{

    unsigned int entryLevel1;
    unsigned int entryLevel2;

    //initialise the page table: fill the 1st-level et necessary 2nd-level tables
    
    int** pointerToFirstLevel = (int**)(kAlloc_aligned(FIRST_LVL_TT_SIZE, 2));
    
        //initialise the frame table
    
    //logical address = physical address for any physical address between 0x0 and __kernel_heap_end__
    entryLevel1=0;
    entryLevel2=0;
    for(entryLevel1=0; entryLevel1<=(__kernel_heap_end__/PAGE_SIZE); entryLevel1++)
    {
        pointerToFirstLevel[entryLevel1] = (int*)kAlloc_aligned(SECON_LVL_TT_SIZE, 2);
        for(entryLevel2=0; entryLevel2<SECON_LVL_TT_COUN; entryLevel2++)
        {
            pointerToFirstLevel[entryLevel1][entryLevel2] = (entryLevel1*SECON_LVL_TT_COUN*FIRST_LVL_TT_COUN+entryLevel2*FIRST_LVL_TT_COUN)|1;
        }
        pointerToFirstLevel[entryLevel1] = (int*)((int)pointerToFirstLevel[entryLevel1]|1);
    }
    
    //logical address = physical adress for any physical address between 0x20000000 and 0x20FFFFFF
    for(entryLevel1=(0x20000000/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN)); entryLevel1<=0x20FFFFFF/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN); entryLevel1++)
    {
        pointerToFirstLevel[entryLevel1] = (int*)kAlloc_aligned(SECON_LVL_TT_SIZE, 2);
        for(entryLevel2=0; entryLevel2<SECON_LVL_TT_COUN; entryLevel2++)
        {
            pointerToFirstLevel[entryLevel1][entryLevel2] = (entryLevel1*SECON_LVL_TT_COUN*FIRST_LVL_TT_COUN+entryLevel2*FIRST_LVL_TT_COUN)|1;
        }
        pointerToFirstLevel[entryLevel1] = (int*)((int)pointerToFirstLevel[entryLevel1]|1);
    }
    
    //physical adresses who are not use
    for(entryLevel1=__kernel_heap_end__/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN)+1; entryLevel1<0x20000000/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN); entryLevel1++)
        pointerToFirstLevel[entryLevel1] = 0;
    for(entryLevel1=0x20FFFFFF/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN)+1; entryLevel1<=0xFFFFFFFF/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN); entryLevel1++)
        pointerToFirstLevel[entryLevel1] = 0;
    
    return (unsigned int)pointerToFirstLevel;
    
}

void
start_mmu_C()
{
register unsigned int control;
__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
__asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) 
__asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries
// Enable ARMv6 MMU features (disable sub-page AP) 
control = (1<<23) | (1 << 15) | (1 << 4) | 1;
// Invalidate the translation lookaside buffer (TLB)
__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
// Write control register 
__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}

void
configure_mmu_C()
{
register unsigned int pt_addr = init_kern_translation_table();
//total++;
// Translation table 0
__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
// Translation table 1 
__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
// Use translation table 0 for everything 
__asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
// Set Domain 0 ACL to "Manager", not enforcing memory permissions
// Every mapped section/page is in domain 0

__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}
