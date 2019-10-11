#include "kheap.c"

void vmem_init(void)
{
        // 1. Configure coprocessor registers.
        configure_mmu_C();       
        
        //2. Configure page table descriptors
        init_kern_translation_table();
    
        //3. De-activate and invalidate the instruction cache.
        //Then, it is possible to re-activate it when activating the MMU.
        // Here is the code for this purpose : mcr p15, 0, r0, c7, c7, 0
}

int
init_kern_translation_table(void)
{
    //initialise the page table: fill the 1st-level et necessary 2nd-level tables
    
    int* pointerToFisrtLevel = kAlloc_aligned(FIRST_LVL_TT_SIZE);
    
    //initialise the frame table
    
    //logical address = physical address for any physical address between 0x0 and __kernel_heap_end__
    //logical address = physical adress for any physical address between 0x20000000 and 0x20FFFFFF
     
    return 0;
}

void
start_mmu_C()
{
register unsigned int control;
__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
__asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) */
__asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries
/* Enable ARMv6 MMU features (disable sub-page AP) */
control = (1<<23) | (1 << 15) | (1 << 4) | 1;
/* Invalidate the translation lookaside buffer (TLB) */
__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
/* Write control register */
__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}

void
configure_mmu_C()
{
register unsigned int pt_addr = MMUTABLEBASE;
total++;
/* Translation table 0 */
__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
/* Translation table 1 */
__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
/* Use translation table 0 for everything */
__asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
/* Set Domain 0 ACL to "Manager", not enforcing memory permissions
* Every mapped section/page is in domain 0
*/
__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}
