#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/ports.h"
#include "../libc/string.h"
#include "process.h"
#include "fs.h"

// Helper: Reboot
void sys_reboot() {
    kprint("Rebooting...\n");
    port_byte_out(0x64, 0xFE);
}

void kernel_main() {
    clear_screen();
    kprint("EduOS Kernel v1.2\n");
    kprint("Type 'help' for commands.\n\n");
    
    init_process_manager();
    init_fs();
    init_keyboard();
    
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

    // --- CD.. FIX (Handle missing space) ---
    if (strcasecmp(input, "cd..") == 0) {
        fs_cd("..");
    }
    // --- STANDARD COMMANDS ---
    else if (strcasecmp(input, "help") == 0) {
        kprint("--- EduOS Help ---\n");
        kprint("File Commands:\n");
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
        kprint("\nSystem Commands:\n");
        kprint("  echo [text]   - Print text\n");
        kprint("  whoami        - Print user\n");
        kprint("  clear         - Clear screen\n");
        kprint("  reboot        - Restart system\n");
        kprint("  monitor       - Task Manager\n");
        kprint("  start         - Start dummy process\n");
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
    else if (strcasecmp(input, "monitor") == 0) { list_processes(); }
    else if (strcasecmp(input, "start") == 0) { create_process("Worker", 1024); }
    else if (strcmp(input, "") == 0) {} 
    else {
        kprint("Unknown command: "); kprint(input); kprint("\n");
    }
    
    // --- UPDATED PROMPT: Shows Current Directory ---
    kprint("root@EduOS:");
    kprint(cwd); // Prints /home/ or /
    kprint("$ ");
}