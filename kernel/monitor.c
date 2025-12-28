#include "monitor.h"
#include "memory.h"
#include "process.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../cpu/idt.h"

void monitor_show_menu() {
    clear_screen();
    kprint("=== EduOS System Monitor ===\n\n");
    kprint("1. Process List\n");
    kprint("2. Memory Map (E820)\n");
    kprint("3. CPU Information\n");
    kprint("4. Back to Shell\n\n");
    kprint("Type 1/2/3 or 4 to return.\n");
}

void monitor_show_processes() {
    kprint("\n[Processes]\n");
    list_processes();
}

void monitor_show_memory() {
    kprint("\n[Memory]\n");
    display_memory_map();
}

void monitor_show_cpu() {
    kprint("\n[CPU]\n");
    kprint("Architecture: x86 (32-bit)\n");
    kprint("IDT Entries: ");
    char buf[16];
    int_to_ascii(IDT_ENTRIES, buf);
    kprint(buf);
    kprint("\nKeyboard IRQ mapped to int 33\n");
}
