#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/ports.h"
#include "../libc/string.h"
#include "process.h"
#include "fs.h"
#include "memory.h"
#include "monitor.h"
#include "education.h"
#include "../drivers/pit.h"
#include "paging.h"
#include "heap.h"
#include "game.h"

// Helper: Reboot
void sys_reboot() {
    kprint("Rebooting...\n");
    port_byte_out(0x64, 0xFE);
}

void kernel_main() {
    clear_screen();
    kprint("EduOS Kernel v1.2\n");
    kprint("Type 'help' for commands.\n\n");
    
    init_paging();
    init_heap();
    init_process_manager();
    init_fs();
    init_keyboard();
    init_pit(1000); // 1000 Hz for ms ticks
    
    kprint("root@EduOS:/$ ");
}

void get_args(char* input, char* arg1, char* arg2) {
    int i = 0;
    while(input[i] != ' ' && input[i] != '\0') i++;
    if (input[i] == ' ') i++;
    int j = 0;
    while(input[i] != ' ' && input[i] != '\0') {
        arg1[j] = input[i]; i++; j++;
    }
    arg1[j] = '\0';
    if (input[i] == ' ') i++;
    j = 0;
    while(input[i] != '\0') {
        arg2[j] = input[i]; i++; j++;
    }
    arg2[j] = '\0';
}

void user_input(char *input) {
    char arg1[20] = ""; 
    char arg2[20] = ""; 
    get_args(input, arg1, arg2); 

    // If game is active, route input to it first
    if (game_is_active()) {
        if (game_handle_input(input) == 0) return; // stay in game, no shell prompt
    }

    // Simple UI mode handling: 0=shell, 1=monitor, 2=education
    static int ui_mode = 0;
    if (ui_mode == 1) {
        if (strcasecmp(input, "1") == 0) { monitor_show_processes(); }
        else if (strcasecmp(input, "2") == 0) { monitor_show_memory(); }
        else if (strcasecmp(input, "3") == 0) { monitor_show_cpu(); }
        else if (strcasecmp(input, "4") == 0 || strcasecmp(input, "back") == 0) { ui_mode = 0; clear_screen(); }
        else { kprint("Use 1/2/3 or 'back' to return.\n"); }
        if (ui_mode == 1) { kprint("\n(Select 1/2/3 or 'back')\n"); }
    }
    else if (ui_mode == 2) {
        if (strcasecmp(input, "back") == 0) { ui_mode = 0; clear_screen(); }
        else if (strlen(input) == 1) { edu_show_topic(input[0]); }
        else { kprint("Use A/B/C/D or 'back'.\n"); }
        if (ui_mode == 2) { kprint("\n(Select A/B/C/D or 'back')\n"); }
    }
    else {

    // --- CD.. FIX (Handle missing space) ---
    if (strcasecmp(input, "cd..") == 0) {
        fs_cd("..");
    }
    // --- STANDARD COMMANDS ---
    else if (strcasecmp(input, "help") == 0) {
        kprint("=== EduOS Help Categories ===\n");
        kprint("Type a category to see commands:\n");
        kprint("  help files    - File system commands\n");
        kprint("  help system   - System info & control\n");
        kprint("  help process  - Process management\n");
        kprint("  help apps     - Applications & games\n");
    }
    else if (strcasecmp(input, "help files") == 0) {
        kprint("=== File System Commands ===\n");
        kprint("  ls            - List files in current dir\n");
        kprint("  pwd           - Print working directory\n");
        kprint("  cd [path]     - Change directory (or ..)\n");
        kprint("  mkdir [name]  - Create directory\n");
        kprint("  touch [name]  - Create file\n");
        kprint("  cat [name]    - Read file\n");
        kprint("  write [n] [t] - Write text to file\n");
        kprint("  rm [name]     - Delete file\n");
        kprint("  cp [src] [dst]- Copy file\n");
        kprint("  mv [old] [new]- Rename/Move file\n");
    }
    else if (strcasecmp(input, "help system") == 0) {
        kprint("=== System Commands ===\n");
        kprint("  echo [text]   - Print text\n");
        kprint("  whoami        - Print user\n");
        kprint("  clear         - Clear screen\n");
        kprint("  reboot        - Restart system\n");
        kprint("  memmap        - Show memory layout\n");
        kprint("  uptime        - Show system uptime (ms)\n");
        kprint("  sleep [ms]    - Busy-wait sleep in milliseconds\n");
    }
    else if (strcasecmp(input, "help process") == 0) {
        kprint("=== Process Management ===\n");
        kprint("  start         - Start dummy process\n");
        kprint("  kill [pid]    - Terminate a process\n");
        kprint("  msg [pid] [c] - Send one-byte IPC to process\n");
        kprint("  inbox [pid]   - Receive one byte from inbox\n");
        kprint("  monitor       - System Monitor (interactive)\n");
    }
    else if (strcasecmp(input, "help apps") == 0) {
        kprint("=== Applications & Games ===\n");
        kprint("  edu           - Learn OS concepts (interactive)\n");
        kprint("  game          - Play a quick guessing game\n");
    }
    else if (strcasecmp(input, "clear") == 0) { clear_screen(); }
    else if (strcasecmp(input, "whoami") == 0) { kprint("root\n"); }
    else if (strcasecmp(input, "reboot") == 0) { sys_reboot(); }
    
    else if (strcasecmp_prefix(input, "echo")) {
        if (strlen(input) > 5) kprint(input + 5);
        kprint("\n");
    }

    else if (strcasecmp(input, "ls") == 0) { fs_list(); }
    else if (strcasecmp(input, "pwd") == 0) { fs_pwd(); }
    
    else if (strcasecmp_prefix(input, "touch")) { 
        if (arg1[0]) fs_create(arg1); else kprint("Usage: touch [file]\n");
    }
    else if (strcasecmp_prefix(input, "mkdir")) { 
        if (arg1[0]) fs_mkdir(arg1); else kprint("Usage: mkdir [name]\n");
    }
    else if (strcasecmp_prefix(input, "cd")) { 
        if (arg1[0]) fs_cd(arg1); else kprint("Usage: cd [path]\n");
    }
    else if (strcasecmp_prefix(input, "cat")) { 
        if (arg1[0]) fs_read(arg1); else kprint("Usage: cat [file]\n");
    }
    else if (strcasecmp_prefix(input, "rm")) { 
        if (arg1[0]) fs_delete(arg1); else kprint("Usage: rm [file]\n");
    }
    else if (strcasecmp_prefix(input, "write")) { 
        if (arg1[0]) fs_write(arg1, "Data written by user"); 
        else kprint("Usage: write [file]\n");
    }
    else if (strcasecmp_prefix(input, "cp")) { 
        if (arg1[0] && arg2[0]) fs_copy(arg1, arg2); else kprint("Usage: cp [src] [dest]\n");
    }
    else if (strcasecmp_prefix(input, "mv")) { 
        if (arg1[0] && arg2[0]) fs_rename(arg1, arg2); else kprint("Usage: mv [old] [new]\n");
    }
    else if (strcasecmp(input, "monitor") == 0) { ui_mode = 1; monitor_show_menu(); }
    else if (strcasecmp(input, "edu") == 0) { ui_mode = 2; edu_show_menu(); }
    else if (strcasecmp(input, "memmap") == 0) { display_memory_map(); }
    else if (strcasecmp(input, "uptime") == 0) { 
        char buf[16]; int_to_ascii(pit_ticks(), buf); kprint(buf); kprint(" ms\n");
    }
    else if (strcasecmp_prefix(input, "sleep")) {
        if (arg1[0]) { 
            int ms = 0; // simple atoi
            int i=0; 
            while (arg1[i]) { 
                if (arg1[i] < '0' || arg1[i] > '9') {
                    kprint("Error: Invalid number.\n");
                    goto skip_sleep;
                }
                ms = ms*10 + (arg1[i]-'0'); 
                i++; 
            }
            if (ms == 0) {
                kprint("Error: Sleep time must be > 0.\n");
                goto skip_sleep;
            }
            kprint("Sleeping...\n");
            pit_sleep(ms);
            kprint("Awake!\n");
            skip_sleep: ;
        } else { kprint("Usage: sleep [ms]\n"); }
    }
    else if (strcasecmp_prefix(input, "kill")) {
        if (arg1[0]) { int pid = 0; int i=0; while (arg1[i]) { pid = pid*10 + (arg1[i]-'0'); i++; }
            terminate_process(pid);
        } else { kprint("Usage: kill [pid]\n"); }
    }
    else if (strcasecmp_prefix(input, "msg")) {
        if (arg1[0] && arg2[0]) {
            int pid = 0; int i=0; while (arg1[i]) { pid = pid*10 + (arg1[i]-'0'); i++; }
            send_message(pid, arg2[0]);
        } else { kprint("Usage: msg [pid] [char]\n"); }
    }
    else if (strcasecmp_prefix(input, "inbox")) {
        if (arg1[0]) {
            int pid = 0; int i=0; while (arg1[i]) { pid = pid*10 + (arg1[i]-'0'); i++; }
            char c;
            if (recv_message(pid, &c)) { char out[2] = {c, '\n'}; kprint(out); }
            else { kprint("(empty)\n"); }
        } else { kprint("Usage: inbox [pid]\n"); }
    }
    else if (strcasecmp(input, "start") == 0) { create_process("Worker", 1024); }
    else if (strcasecmp(input, "game") == 0) { game_start(); if (game_is_active()) return; }
    else if (strcmp(input, "") == 0) {} 
    else {
        kprint("Unknown command: "); kprint(input); kprint("\n");
    }
    }

    // --- UPDATED PROMPT: Shows Current Directory ---
    kprint("root@EduOS:");
    kprint(cwd); // Prints /home/ or /
    kprint("$ ");
}