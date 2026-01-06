#include "process.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

Process process_list[MAX_PROCESSES];
int process_count = 0;
static int current_index = 0;

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
    p.run = 0;
    p.inbox_head = p.inbox_tail = 0;
    p.cr3 = 0;
    p.eax = p.ecx = p.edx = p.ebx = p.esp_save = p.ebp = p.esi = p.edi = 0;

    process_list[process_count] = p;
    process_count++;
}

void list_processes() {
    kprint("\nPID   | STATE | MEMORY | NAME\n");
    kprint("----------------------------------\n");
    
    char buffer[20]; // Buffer for number-to-string conversion
    
    for (int i = 0; i < process_count; i++) {
        Process p = process_list[i];
        
        // Skip terminated processes
        if (p.state == TERMINATED) continue;
        
        // Print PID
        int_to_ascii(p.pid, buffer);
        kprint(buffer);
        kprint("  |   ");
        
        // Print State
        if (p.state == RUNNING) kprint("RUN");
        else if (p.state == READY) kprint("RDY");
        else if (p.state == BLOCKED) kprint("BLK");
        else kprint("???");
        
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

void terminate_process(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid) {
            // Prevent killing critical system processes
            if (i == 0) {
                kprint("Error: Cannot kill KERNEL process.\n");
                return;
            }
            if (i == 1) {
                kprint("Error: Cannot kill SHELL process.\n");
                return;
            }
            
            process_list[i].state = TERMINATED;
            kprint("Process terminated: "); kprint(process_list[i].name); kprint("\n");
            return;
        }
    }
    kprint("Error: PID not found.\n");
}

// Very simple round-robin scheduler skeleton: rotates RUNNING among READY processes
void schedule_tick() {
    if (process_count == 0) return;

    // Advance to next index
    int next = (current_index + 1) % process_count;
    // Skip terminated processes
    int attempts = 0;
    while (process_list[next].state == TERMINATED && attempts < process_count) {
        next = (next + 1) % process_count;
        attempts++;
    }
    // Mark current READY, next RUNNING (if not terminated)
    if (process_list[current_index].state == RUNNING)
        process_list[current_index].state = READY;

    if (process_list[next].state != TERMINATED)
        process_list[next].state = RUNNING;

    current_index = next;

    // Simulate a task "run" if provided
    if (process_list[current_index].run) {
        process_list[current_index].run();
    }
}

int send_message(int pid, char c) {
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid && process_list[i].state != TERMINATED) {
            int next_tail = (process_list[i].inbox_tail + 1) % 128;
            if (next_tail == process_list[i].inbox_head) {
                kprint("Mailbox full.\n");
                return 0;
            }
            process_list[i].inbox[process_list[i].inbox_tail] = c;
            process_list[i].inbox_tail = next_tail;
            return 1;
        }
    }
    kprint("Error: PID not found.\n");
    return 0;
}

int recv_message(int pid, char* c) {
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid && process_list[i].state != TERMINATED) {
            if (process_list[i].inbox_head == process_list[i].inbox_tail) {
                return 0; // empty
            }
            *c = process_list[i].inbox[process_list[i].inbox_head];
            process_list[i].inbox_head = (process_list[i].inbox_head + 1) % 128;
            return 1;
        }
    }
    return 0;
}

int process_current_index() { return current_index; }
Process* process_current() { return &process_list[current_index]; }
void process_set_current_index(int idx) { current_index = idx; }