#include "education.h"
#include "../drivers/screen.h"

void edu_show_menu() {
    clear_screen();
    kprint("--- OS Concepts Explained ---\n\n");
    kprint("A. Process Scheduling\n");
    kprint("B. Memory Management\n");
    kprint("C. File Systems\n");
    kprint("D. System Calls\n\n");
    kprint("Type A/B/C/D or 'back' to return.\n");
}

void edu_show_topic(char key) {
    switch (key) {
        case 'A': case 'a':
            kprint("\n[Process Scheduling]\n");
            kprint("Schedulers decide which process runs next.\n");
            kprint("Policies include Round Robin, Priority, and Multilevel queues.\n\n");
            break;
        case 'B': case 'b':
            kprint("\n[Memory Management]\n");
            kprint("OS manages RAM via segmentation/paging, allocates/free blocks, and prevents overlap.\n\n");
            break;
        case 'C': case 'c':
            kprint("\n[File Systems]\n");
            kprint("Organize data using directories and files; metadata tracks names, sizes, and locations.\n\n");
            break;
        case 'D': case 'd':
            kprint("\n[System Calls]\n");
            kprint("User programs request OS services (I/O, processes, memory) via trap/interrupt gates.\n\n");
            break;
        default:
            kprint("\nUnknown topic. Use A/B/C/D or 'back'.\n");
            break;
    }
}
