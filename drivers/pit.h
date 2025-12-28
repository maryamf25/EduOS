#ifndef PIT_H
#define PIT_H

#include "../kernel/types.h"

void init_pit(u32 frequency);
void isr_timer_handler(u32* regs_esp);
u32 pit_ticks();
void pit_sleep(u32 ms);

#endif
