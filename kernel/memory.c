#include "memory.h"
#include "../drivers/screen.h"
#include "types.h"

typedef struct {
    u32 base_low;     // lower 32 bits of base
    u32 base_high;    // upper 32 bits of base
    u32 length_low;   // lower 32 bits of length
    u32 length_high;  // upper 32 bits of length
    u32 type;         // 1=Usable, 2=Reserved, 3=ACPI reclaimable, 4=ACPI NVS, 5=Bad
    u32 acpi;         // unused here
} __attribute__((packed)) e820_entry_t;

void print_hex32(u32 v) {
    char hex[11];
    const char *digits = "0123456789ABCDEF";
    hex[0] = '0'; hex[1] = 'x';
    for (int i = 0; i < 8; i++) {
        int shift = (7 - i) * 4;
        hex[2+i] = digits[(v >> shift) & 0xF];
    }
    hex[10] = '\0';
    kprint(hex);
}

void display_memory_map() {
    kprint("\nMemory Layout (BIOS E820):\n");

    volatile u32 *count_ptr = (u32*)0x5000;   // written by bootloader
    volatile e820_entry_t *entries = (e820_entry_t*)0x5004;
    u32 count = *count_ptr;

    if (count == 0) {
        kprint("(No E820 entries found)\n\n");
        return;
    }

    for (u32 i = 0; i < count; i++) {
        e820_entry_t e = entries[i];

        // Show only lower 32 bits to keep printing simple
        kprint("Base="); print_hex32(e.base_low);
        kprint("  Length="); print_hex32(e.length_low);
        kprint("  Type=");
        switch (e.type) {
            case 1: kprint("Usable\n"); break;
            case 2: kprint("Reserved\n"); break;
            case 3: kprint("ACPI Reclaimable\n"); break;
            case 4: kprint("ACPI NVS\n"); break;
            case 5: kprint("Bad Memory\n"); break;
            default: kprint("Unknown\n"); break;
        }
    }
    kprint("\n");
}
