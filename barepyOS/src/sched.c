#include "sched.h"

struct pcb_s *current_process;
struct pcb_s kmain_process;

void
sched_init(void)
{
    current_process = &kmain_process;
}
