#include "kheap.h"
#include "vmem.h"
#include "hw.h"


void vmem_init(void)
{
    //init physical memory
    kheap_init();
    
        //Configure coprocessor registers.
    configure_mmu_C();
        
        //activate interrupts
    ENABLE_IRQ();
        //3. De-activate and invalidate the instruction cache.
        //Then, it is possible to re-activate it when activating the MMU.
        // Here is the code for this purpose : mcr p15, 0, r0, c7, c7, 0
    unsigned int test = vmem_translate(0x20000000, NULL);
    test++;
        
    start_mmu_C();
}

unsigned int
init_kern_translation_table(void)
{

    unsigned int entryLevel1;
    unsigned int entryLevel2;

    //initialise the page table: fill the 1st-level et necessary 2nd-level tables
    
    int** pointerToFirstLevel = (int**)(kAlloc_aligned(FIRST_LVL_TT_SIZE, 14));
    
        //initialise the frame table
    
    //logical address = physical address for any physical address between 0x0 and __kernel_heap_end__
    entryLevel1=0;
    entryLevel2=0;
    for(entryLevel1=0; entryLevel1<=(__kernel_heap_end__/PAGE_SIZE); entryLevel1++)
    {
        pointerToFirstLevel[entryLevel1] = (int*)kAlloc_aligned(SECON_LVL_TT_SIZE, 10);
        for(entryLevel2=0; entryLevel2<SECON_LVL_TT_COUN; entryLevel2++)
        {
            pointerToFirstLevel[entryLevel1][entryLevel2] = (entryLevel1*SECON_LVL_TT_COUN*FIRST_LVL_TT_COUN+entryLevel2*FIRST_LVL_TT_COUN)|1;
        }
        pointerToFirstLevel[entryLevel1] = (int*)((int)pointerToFirstLevel[entryLevel1]|1);
    }
    
    //logical address = physical adress for any physical address between 0x20000000 and 0x20FFFFFF
    for(entryLevel1=(0x20000000/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN)); entryLevel1<=0x20FFFFFF/(FIRST_LVL_TT_COUN*SECON_LVL_TT_COUN); entryLevel1++)
    {
        pointerToFirstLevel[entryLevel1] = (int*)kAlloc_aligned(SECON_LVL_TT_SIZE, 10);
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

uint32_t
vmem_translate(uint32_t va, struct pcb_s * process)
{
    uint32_t pa; /* The result */
    /* 1st and 2nd table addresses */
    uint32_t table_base;
    uint32_t second_level_table;
    /* Indexes */
    uint32_t first_level_index;
    uint32_t second_level_index;
    uint32_t page_index;
    /* Descriptors */
    uint32_t first_level_descriptor;
    uint32_t* first_level_descriptor_address;
    uint32_t second_level_descriptor;
    uint32_t* second_level_descriptor_address;
    if (process == NULL)
    {
        __asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
    }
    else
    {
        //table_base = (uint32_t) process->page_table;
    }
    table_base = table_base & 0xFFFFC000;
    /* Indexes*/
    first_level_index = (va >> 20);
    second_level_index = ((va << 12) >> 24);
    page_index = (va & 0x00000FFF);
    /* First level descriptor */
    first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
    first_level_descriptor = *(first_level_descriptor_address);
    /* Translation fault*/
    if (! (first_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Second level descriptor */
    second_level_table = first_level_descriptor & 0xFFFFFC00;
    second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
    second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
    /* Translation fault*/
    if (! (second_level_descriptor & 0x3)) {
        return (uint32_t) FORBIDDEN_ADDRESS;
    }
    /* Physical address */
    pa = (second_level_descriptor & 0xFFFFF000) | page_index;
    return pa;
}
