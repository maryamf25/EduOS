#include "process.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

Process process_list[MAX_PROCESSES];
int process_count = 0;

void init_process_manager() {
    // Create a base process so the list isn't empty
    create_process("KERNEL", 4096);
    process_list[0].state = RUNNING; // The kernel is always running
    
    create_process("SHELL", 2048);
    process_list[1].state = RUNNING;
}

void create_process(char* name, int memory) {
    if (process_count >= MAX_PROCESSES) {
        kprint("Error: Max processes reached.\n");
        return;
    }

    Process p;
    p.pid = 1000 + process_count; // PIDs start at 1000
    // Manually copy string since we don't have strcpy yet
    int i = 0;
    while(name[i] != '\0' && i < 19) {
        p.name[i] = name[i];
        i++;
    }
    p.name[i] = '\0';
    
    p.state = READY;
    p.memory_usage = memory;

    process_list[process_count] = p;
    process_count++;
}

void list_processes() {
    kprint("\nPID   | STATE | MEMORY | NAME\n");
    kprint("----------------------------------\n");
    
    char buffer[20]; // Buffer for number-to-string conversion
    
    for (int i = 0; i < process_count; i++) {
        Process p = process_list[i];
        
        // Print PID
        int_to_ascii(p.pid, buffer);
        kprint(buffer);
        kprint("  |   ");
        
        // Print State
        if (p.state == RUNNING) kprint("RUN");
        else if (p.state == READY) kprint("RDY");
        else kprint("BLK");
        
        kprint("   |  ");
        
        // Print Memory
        int_to_ascii(p.memory_usage, buffer);
        kprint(buffer);
        kprint("B");
        
        // Align spacing (simple tab simulation)
        if (p.memory_usage < 1000) kprint("   | ");
        else kprint("  | ");

        kprint(p.name);
        kprint("\n");
    }
    kprint("\n");
}