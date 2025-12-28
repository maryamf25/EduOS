#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

// Process States
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define TERMINATED 3

#define MAX_PROCESSES 10

typedef struct {
    int pid;          // Process ID (e.g., 1001)
    char name[20];    // Name (e.g., "Shell")
    int state;        // 0=Ready, 1=Running...
    int memory_usage; // Fake memory usage in bytes
    void (*run)();    // Simple entry function (simulated task)
    int inbox_head;   // IPC inbox cursor
    int inbox_tail;   // IPC inbox cursor
    char inbox[128];  // Simple byte queue as mailbox
    u32 cr3;          // Page directory physical address for isolation
    // Saved register context (matching pusha order)
    u32 eax, ecx, edx, ebx, esp_save, ebp, esi, edi;
} Process;

void init_process_manager();
void create_process(char* name, int memory);
void list_processes();
void terminate_process(int pid);
void schedule_tick();
int send_message(int pid, char c);
int recv_message(int pid, char* c);
int process_current_index();
Process* process_current();
void process_set_current_index(int idx);

#endif