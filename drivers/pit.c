#include "pit.h"
#include "ports.h"
#include "../cpu/idt.h"
#include "../kernel/types.h"
#include "../kernel/process.h"

static volatile u32 ticks = 0;

u32 pit_ticks() { return ticks; }

void pit_set_frequency(u32 hz) {
    // PIT base frequency is 1193182 Hz
    u32 divisor = 1193182 / hz;
    // Command port 0x43, channel 0, lobyte/hibyte, mode 3 (square wave)
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, (u8)(divisor & 0xFF));       // low byte
    port_byte_out(0x40, (u8)((divisor >> 8) & 0xFF)); // high byte
}

void init_pit(u32 frequency) {
    pit_set_frequency(frequency);

    extern void isr_timer();
    set_idt_gate(32, (u32)isr_timer); // IRQ0 -> IDT 32
    set_idt();

    // Unmask IRQ0 (timer) and IRQ1 (keyboard)
    port_byte_out(0x21, 0xFC); // 11111100 -> allow IRQ0, IRQ1
    port_byte_out(0xA1, 0xFF); // mask all on slave PIC
}

void isr_timer_handler(u32* regs_esp) {
    (void)regs_esp; // currently unused in this skeleton
    ticks++;
    schedule_tick();
    port_byte_out(0x20, 0x20);
}

void pit_sleep(u32 ms) {
    if (ms == 0) return;
    
    u32 start = ticks;
    u32 elapsed = 0;
    
    // Avoid overflow issues by checking elapsed time instead of target
    while (elapsed < ms) {
        u32 current = ticks;
        // Handle tick counter wrap-around
        if (current >= start) {
            elapsed = current - start;
        } else {
            // Overflow occurred
            elapsed = (0xFFFFFFFF - start) + current + 1;
        }
    }
}
