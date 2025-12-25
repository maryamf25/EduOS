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
} Process;

void init_process_manager();
void create_process(char* name, int memory);
void list_processes();

#endif