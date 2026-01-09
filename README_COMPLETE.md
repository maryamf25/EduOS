# EduOS - Educational 32-bit Operating System

A pedagogical x86 operating system demonstrating fundamental OS concepts including memory management, process scheduling, interrupt handling, device drivers, and file systems.

## 📖 Table of Contents

- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [System Architecture](#system-architecture)
- [Boot Process](#boot-process)
- [OS Concepts Implemented](#os-concepts-implemented)
- [Directory Structure](#directory-structure)
- [Memory Layout](#memory-layout)
- [Command Reference](#command-reference)
- [Build & Run](#build--run)
- [Technical Details](#technical-details)
- [Presentation Guide](#presentation-guide)

---

## 🎯 Project Overview

**EduOS** is a complete 32-bit operating system built from scratch in C and x86 assembly. It boots from a custom bootloader, initializes memory management through paging and heap allocation, implements process scheduling and inter-process communication, handles hardware interrupts, and provides a functional file system with a shell interface.

**Target**: Educational demonstration of OS internals and core concepts for computer science students.

**Platform**: x86-32 (IA-32) Protected Mode

**Language**: C, x86 Assembly

**Build System**: Makefile

---

## ✨ Key Features

### Core OS Features
- ✅ **Protected Mode Boot** - Custom bootloader with GDT and E820 memory detection
- ✅ **Paging & Virtual Memory** - 2-level page tables with per-process isolation
- ✅ **Dynamic Memory Allocation** - Free-list heap allocator (256 KB)
- ✅ **Process Management** - Up to 10 concurrent processes with PCBs
- ✅ **Preemptive Scheduling** - Round-robin scheduler driven by timer interrupts
- ✅ **Interrupt Handling** - IDT with 256 entries, hardware interrupt support
- ✅ **File System** - Hierarchical in-memory FS with CRUD operations
- ✅ **Inter-Process Communication** - Mailbox-based message passing

### Hardware Drivers
- ✅ **Keyboard Driver** - PS/2 scancode translation, line editing, modifiers
- ✅ **VGA Text Mode Driver** - Direct memory I/O, cursor control, scrolling
- ✅ **PIT Timer Driver** - Programmable interval timer (IRQ0)
- ✅ **Port I/O Abstraction** - Low-level I/O port operations

### User-Facing Features
- ✅ **Command-line Shell** - Interactive prompt with command parsing
- ✅ **Educational Module** - Built-in OS concepts tutorial
- ✅ **System Monitor** - Live process, memory, and CPU information display
- ✅ **Game Application** - "Guess the Number" demonstrating user interaction
- ✅ **Help System** - Categorized command documentation

---

## 🏗️ System Architecture

### Layered Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                          USER SPACE / SHELL                          │
│  ┌──────────────┬──────────────┬──────────────┬──────────────┐     │
│  │ Shell/CLI    │ File System  │   Games      │  Monitor     │     │
│  │ Commands     │ Operations   │ Applications │   Tools      │     │
│  └──────────────┴──────────────┴──────────────┴──────────────┘     │
└───────────────────────────────┬─────────────────────────────────────┘
                                │ System Call Interface
┌───────────────────────────────┴─────────────────────────────────────┐
│                         KERNEL LAYER                                 │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │         kernel.c - Main Event Loop & Command Handler          │   │
│  └──────┬────────────┬────────────┬────────────┬────────────────┘   │
│         │            │            │            │                     │
│  ┌──────▼──────┬─────▼─────┬─────▼─────┬──────▼──────┬──────────┐  │
│  │ Process Mgr │ File Sys  │  Memory   │   Monitor   │  Apps    │  │
│  │ process.c   │   fs.c    │ memory.c  │  monitor.c  │ game.c   │  │
│  └──────┬───────┴──────┬────┴─────┬─────┴──────┬──────┴─────┬────┘  │
│         │              │          │            │            │        │
│  ┌──────▼──────────────▼──────────▼────────────▼─┐          │       │
│  │        MEMORY MANAGEMENT LAYER                │          │       │
│  │  ┌──────────────┬─────────────────┐          │          │       │
│  │  │ Heap         │ Paging          │          │          │       │
│  │  │ heap.c       │ paging.c        │          │          │       │
│  │  │ - kmalloc    │ - init_paging   │          │          │       │
│  │  │ - kfree      │ - map_page      │          │          │       │
│  │  │ - Free List  │ - Page Dir/Tab  │          │          │       │
│  │  └──────────────┴─────────────────┘          │          │       │
│  └───────────────────────────────────────────────┘          │       │
│                                                              │       │
│  ┌──────────────────────────────────────────────────────────┤       │
│  │        INTERRUPT & DRIVER LAYER                           │       │
│  │  ┌─────────────────┬──────────────────────────────────┐   │       │
│  │  │ IDT (cpu/idt.c) │  Drivers (drivers/)              │   │       │
│  │  │ - set_idt_gate  │  - keyboard.c (IRQ1)             │   │       │
│  │  │ - 256 entries   │  - pit.c (IRQ0)                  │   │       │
│  │  │                 │  - screen.c (VGA)                │   │       │
│  │  │                 │  - ports.c (I/O abstraction)     │   │       │
│  │  └─────────────────┴──────────────────────────────────┘   │       │
│  └───────────────────────────────────────────────────────────┘       │
└───────────────────────────────┬─────────────────────────────────────┘
                                │
┌───────────────────────────────▼─────────────────────────────────────┐
│                          HARDWARE LAYER                              │
│  ┌──────────────┬──────────────┬──────────────┬──────────────────┐  │
│  │   CPU x86    │  RAM (Phys)  │  Keyboard    │  VGA Display    │  │
│  │ - CR0, CR3   │ - E820 Map   │ - Port 0x60  │ - 0xB8000       │  │
│  │ - IDT/GDT    │ - Paging MMU │ - Scancodes  │ - Text Mode     │  │
│  │ - Interrupts │              │              │ (80x25 chars)   │  │
│  └──────────────┴──────────────┴──────────────┴──────────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 🚀 Boot Process

### Phase 1: Bootloader (boot/boot.asm)
```
Power On
  ↓
BIOS POST & MBR Load
  ↓
Boot Code Execution (16-bit Real Mode)
  ├─ Query BIOS E820 memory map → Store at 0x5000
  ├─ Load GDT (Global Descriptor Table)
  ├─ Switch to 32-bit Protected Mode
  └─ Load kernel from disk to 0x10000
  ↓
Jump to kernel_entry.asm
```

### Phase 2: Kernel Entry (boot/kernel_entry.asm)
```
Set up kernel stack
  ↓
Call kernel_main() in C
```

### Phase 3: Kernel Initialization (kernel/kernel.c)
```
kernel_main() Execution Order:

1. Clear Screen & Print Boot Banner
   └─ "EduOS Kernel v1.2"

2. init_paging()
   ├─ Create Page Directory (1024 entries)
   ├─ Create 4 Page Tables (4096 total entries)
   ├─ Identity map 0x00000000 - 0x00FFFFFF (16 MB)
   ├─ Load CR3 with PD address
   ├─ Set CR0.PG = 1 (enable paging)
   └─ Result: Virtual memory enabled

3. init_heap()
   ├─ Initialize free_list with 256 KB block
   └─ Result: kmalloc/kfree ready

4. init_process_manager()
   ├─ Create KERNEL process (PID=1000, state=RUNNING)
   ├─ Create SHELL process (PID=1001, state=RUNNING)
   └─ Result: Process table initialized

5. init_fs()
   ├─ Create root directory (cwd="/")
   ├─ Create readme.txt with welcome message
   └─ Result: File system operational

6. init_keyboard()
   ├─ Remap PIC (IRQ1 → INT 33)
   ├─ Register isr_keyboard handler
   ├─ Unmask keyboard interrupt
   └─ Result: Keyboard input enabled

7. init_pit(1000)
   ├─ Set timer frequency to 1000 Hz (1 ms)
   ├─ Register isr_timer handler at INT 32
   ├─ Unmask timer interrupt
   └─ Result: Timer fires every 1 ms

8. Enable Interrupts
   └─ __asm__ volatile("sti");

9. Show Shell Prompt
   └─ "root@EduOS:/$ "
```

### Phase 4: Runtime Operation (Interrupt-Driven)

```
Main Event Loop:

CPU Idle
  ├──→ [IRQ0: Timer Interrupt - Every 1ms]
  │    1. isr_timer() called
  │    2. Increment ticks counter
  │    3. Call schedule_tick() → Round-robin scheduler
  │    4. Send EOI to PIC (port 0x20)
  │
  ├──→ [IRQ1: Keyboard Interrupt - On Key Press]
  │    1. isr_keyboard() called
  │    2. Read scancode from port 0x60
  │    3. Translate to ASCII (with Shift/Caps Lock handling)
  │    4. Handle special keys (Backspace, Arrows, Delete, Enter)
  │    5. Update key_buffer and cursor position
  │    6. On ENTER: Call user_input(key_buffer)
  │    7. Send EOI to PIC
  │
  └──→ [user_input() - Command Processing]
       1. Parse command and arguments
       2. Match command to handler function
       3. Execute handler (file ops, process ops, system calls)
       4. Display result via kprint()
       5. Show prompt: "root@EduOS:/$ "
       6. Return to idle
```

---

## 📚 OS Concepts Implemented

### 1. Memory Management

#### A. Paging (Virtual Memory)

**File**: `kernel/paging.c`

**What Implemented**:
- 2-level page table hierarchy
- Page Directory: 1024 entries × 4 bytes = 4 KB
- Page Tables: 4 tables × 1024 entries = 4096 mappings total
- Address translation: 10-bit dir index + 10-bit table index + 12-bit offset
- Identity mapping: Virtual == Physical for first 16 MB
- Per-process address spaces via `create_address_space()`
- Context switching via `switch_address_space()` (loads CR3)

**Key Functions**:
```c
void init_paging();                              // Enable paging
page_dir_t create_address_space();               // New process PD
void switch_address_space(page_dir_t pd_phys);   // Context switch
void map_page(page_dir_t pd, u32 virt, u32 phys, int writable);  // Map VA→PA
u32 alloc_frame();                               // Allocate 4 KB page
```

**How to Demonstrate**:
```bash
memmap          # Show E820 memory map
monitor → 1     # Show process isolation (each has own PD)
start           # Create new process (allocates page dir)
```

**What to Tell Teacher**:
> "I implemented x86 hardware paging with 2-level page tables. The MMU translates virtual addresses using the Page Directory (CR3 register) and Page Tables. Each process gets an isolated 4GB virtual address space, but shares kernel mappings for efficiency. I use identity mapping for the first 16MB to access hardware directly."

---

#### B. Heap Allocation

**File**: `kernel/heap.c`

**What Implemented**:
- Free-list allocator (linked list of blocks)
- First-fit allocation strategy
- Block splitting to reduce fragmentation
- Block coalescing on free to prevent fragmentation
- 256 KB total heap
- Block metadata: size, free flag, next pointer

**Key Functions**:
```c
void init_heap();           // Initialize empty heap
void* kmalloc(u32 size);    // Allocate memory
void kfree(void* ptr);      // Free memory
```

**Internal Functions**:
```c
void split_block(Block* blk, u32 size);  // Split oversized block
void coalesce();                          // Merge adjacent free blocks
```

**How to Demonstrate**:
```bash
touch file1.txt             # Allocate FileNode
mkdir dir1                  # Allocate another node
touch file2.txt
rm file2.txt                # Free node (coalesce)
ls                          # List remaining
```

**What to Tell Teacher**:
> "I implemented a 256KB heap using a free-list allocator with first-fit allocation. Each block has metadata tracking size and free status. To prevent fragmentation, I split oversized blocks during allocation and coalesce adjacent free blocks during deallocation. This is similar to Doug Lea's malloc but simplified for educational purposes."

---

### 2. Process Management

#### A. Process Scheduler

**File**: `kernel/process.c`

**What Implemented**:
- Process Control Block (PCB) structure with:
  - PID, name, state, memory usage
  - CPU context (eax, ebx, ecx, edx, esp, ebp, esi, edi)
  - Page directory (cr3) for address space isolation
  - IPC mailbox (128-byte circular buffer)
- Round-robin scheduler (`schedule_tick()` called by timer)
- Process states: READY, RUNNING, BLOCKED, TERMINATED
- Process table: array of 10 max processes

**Key Functions**:
```c
void init_process_manager();      // Create KERNEL & SHELL
void create_process(char* name, int memory);  // New process
void list_processes();            // Display process table
void terminate_process(int pid);  // Kill process
void schedule_tick();             // Round-robin scheduling (called by timer)
```

**How to Demonstrate**:
```bash
monitor                     # Show system monitor
1                          # Display process list
start                      # Create new process
1                          # Process appears with state=READY
kill 1002                  # Terminate process
1                          # Process marked TERMINATED
```

**What to Tell Teacher**:
> "I implemented preemptive multitasking with round-robin scheduling. The PIT timer triggers IRQ0 every millisecond, calling schedule_tick() which rotates among READY processes. Each process has a PCB storing its state, including saved CPU registers for context switching. The scheduler skips TERMINATED processes and maintains fairness through circular iteration."

---

#### B. Inter-Process Communication (IPC)

**File**: `kernel/process.c`

**What Implemented**:
- Mailbox IPC model
- Each process has 128-byte circular buffer
- Send message: `send_message(pid, char)`
- Receive message: `recv_message(pid, &char)`
- Head/tail pointers for circular queue management

**Key Functions**:
```c
int send_message(int pid, char c);     // Send 1-byte message
int recv_message(int pid, char* c);    // Receive 1-byte message
```

**How to Demonstrate**:
```bash
send_message(1001, 'X')   # Put 'X' in PID 1001's mailbox
recv_message(1001)        # Get message from mailbox
```

**What to Tell Teacher**:
> "I implemented asynchronous message passing via mailboxes. Each process has a 128-byte circular queue. Processes can send single-byte messages to any PID using send_message(), and recipients poll their inbox with recv_message(). This demonstrates producer-consumer communication patterns without shared memory."

---

### 3. Interrupt Handling

#### A. Interrupt Descriptor Table (IDT)

**File**: `cpu/idt.c` + `cpu/interrupt.asm`

**What Implemented**:
- IDT structure with 256 entries (32 exceptions + 224 interrupts)
- Each entry: offset (16 bits lo + 16 bits hi), selector, flags
- Gate types: Interrupt gates (disable IF during handler)
- PIC remapping: IRQ0-15 → INT 32-47 (avoid CPU exceptions)
- EOI (End of Interrupt) protocol

**Key Functions**:
```c
void set_idt_gate(int n, u32 handler);  // Set IDT entry
void set_idt();                          // Load IDT into CPU
```

**Interrupt Mapping**:
```
IRQ0 (Timer)    → INT 32  (isr_timer)
IRQ1 (Keyboard) → INT 33  (isr_keyboard)
INT 0-31 reserved for CPU exceptions
```

**How to Demonstrate**:
```bash
monitor
3               # Show CPU info (displays IDT_ENTRIES)
uptime          # Timer interrupt (IRQ0) incrementing ticks
# Type anything → Keyboard interrupt (IRQ1) handling
```

**What to Tell Teacher**:
> "I set up the x86 IDT with 256 entries to handle interrupts and exceptions. I remapped the PIC so hardware IRQs don't conflict with CPU exceptions. IRQ0 (timer) maps to INT 32, IRQ1 (keyboard) to INT 33. Each interrupt handler saves CPU state, processes the interrupt, sends EOI to the PIC, and restores state before returning."

---

#### B. Timer Interrupt (PIT - Programmable Interval Timer)

**File**: `drivers/pit.c`

**What Implemented**:
- Configure PIT frequency via ports 0x43 (control), 0x40 (data)
- Divisor formula: 1193182 / desired_frequency
- Tick counter (volatile u32) incremented on each IRQ0
- pit_sleep() for millisecond-precision delays
- Scheduler integration: calls `schedule_tick()` on each interrupt

**Key Functions**:
```c
void init_pit(u32 frequency);      // Initialize with frequency
void isr_timer_handler(u32* regs);  // IRQ0 handler
u32 pit_ticks();                    // Get current tick count
void pit_sleep(u32 ms);             // Sleep N milliseconds
```

**How to Demonstrate**:
```bash
uptime                  # Show milliseconds since boot
uptime                  # Wait and run again → see increase
sleep 2000              # Sleep 2 seconds
uptime                  # Another 2000+ ms added
```

**What to Tell Teacher**:
> "I programmed the 8253/8254 PIT to generate interrupts at 1000 Hz (1ms resolution). The divisor is calculated as 1193182 / frequency and sent to the control ports. Each IRQ0 increments a tick counter used for uptime tracking and implements pit_sleep() for precise delays. This also drives the process scheduler."

---

#### C. Keyboard Interrupt (IRQ1)

**File**: `drivers/keyboard.c`

**What Implemented**:
- Scancode-to-ASCII translation via lookup table
- Modifier key tracking: Shift, Caps Lock (with XOR logic)
- Extended scancode handling (0xE0 prefix for arrow keys)
- Line editing: Backspace, Delete, arrow navigation, cursor positioning
- Character insertion/deletion at cursor (not just append)
- Input buffer: 256 bytes with cursor position tracking

**Key Functions**:
```c
void isr_keyboard_handler();   // IRQ1 handler
void init_keyboard();          // Setup keyboard interrupt
```

**How to Demonstrate**:
```bash
# Type with normal case
hello

# Type with Shift (XOR with Caps Off = uppercase)
HELLO

# Enable Caps Lock
CapsLock + abc          # → ABC
Shift + abc with Caps   # → abc (XOR logic)

# Line editing
test<Left><Left>x       # → texst (insert at cursor)
<Backspace>             # → text (delete before cursor)
<Right><Right><Delete>  # → tet (delete at cursor)
```

**What to Tell Teacher**:
> "I wrote a full-featured keyboard driver handling PS/2 scancodes from port 0x60. It tracks modifier state (Shift, Caps Lock) and implements XOR logic for uppercase (Shift XOR Caps). Extended scancodes (0xE0 prefix) enable arrow key navigation. The driver maintains a cursor position and allows inserting/deleting characters mid-buffer, not just appending."

---

### 4. Device Drivers

#### A. VGA Text Mode Screen Driver

**File**: `drivers/screen.c`

**What Implemented**:
- Direct memory writes to 0xB8000 (VGA text buffer)
- 80×25 character grid in text mode
- Character format: ASCII (1 byte) + attribute (1 byte)
- Cursor control via CRT controller ports (0x3D4/0x3D5)
- Auto-scrolling when text exceeds screen height
- Scrolling algorithm: shift rows up, clear last line

**Key Functions**:
```c
void kprint(char* message);                    // Print at cursor
void kprint_at(char* msg, int col, int row);   // Print at position
void clear_screen();                           // Clear & reset cursor
int get_cursor_offset();                       // Read cursor position
void set_cursor_offset(int offset);            // Write cursor position
void kprint_backspace();                       // Delete character
void kprint_move_cursor(int direction);        // Move ±1 position
int handle_scrolling(int cursor_offset);       // Auto-scroll
```

**How to Demonstrate**:
```bash
clear                   # Clear screen, reset cursor
echo Hello World        # Print text
help                    # Long output → triggers scrolling
monitor → 2             # Display memory info
help files              # Another scroll example
```

**What to Tell Teacher**:
> "I implemented a VGA text mode driver interfacing with the 0xB8000 memory-mapped buffer. Each character uses 2 bytes (ASCII + attribute). Cursor position is read/written via CRT controller I/O ports. I added auto-scrolling by copying rows upward when the screen fills, similar to Unix TTY behavior."

---

#### B. Port I/O Abstraction Layer

**File**: `drivers/ports.c`

**What Implemented**:
- `port_byte_in(port)`: Read byte from I/O port
- `port_byte_out(port, data)`: Write byte to I/O port
- Inline x86 assembly for `in`/`out` instructions

**Key Functions**:
```c
unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
```

**What to Tell Teacher**:
> "I abstracted x86 I/O port operations using inline assembly. The `in` instruction reads from hardware ports (e.g., 0x60 for keyboard), and `out` writes to them (e.g., 0x20 for PIC EOI). This provides a clean C interface to low-level hardware communication."

---

### 5. File System

**File**: `kernel/fs.c`

**What Implemented**:
- Hierarchical directory structure (/, /home/, /data/, etc.)
- Inode-like FileNode structure with:
  - Full path string
  - Data payload (1024 bytes max)
  - Type: FS_FILE or FS_DIR
  - Size and linked list pointer
- Path resolution: absolute (`/home/file.txt`) and relative (`file.txt`)
- Parent directory navigation (`cd ..`)
- Current working directory (cwd) tracking
- Linked list storage in heap memory

**Key Functions**:
```c
void init_fs();                          // Initialize root
void fs_list();                          // List current directory
int fs_create(char* name);               // Create file
int fs_mkdir(char* name);                // Create directory
int fs_cd(char* path);                   // Change directory
void fs_pwd();                           // Print working directory
int fs_write(char* name, char* data);    // Write file contents
void fs_read(char* name);                // Read file contents
void fs_delete(char* name);              // Delete file
void fs_copy(char* src, char* dest);     // Copy file
void fs_rename(char* src, char* dest);   // Rename file
```

**How to Demonstrate**:
```bash
pwd                     # Show current directory
ls                      # List files
mkdir home              # Create directory
cd home                 # Change to it
pwd                     # Now in /home/
touch file.txt          # Create file
write file.txt          # Write content
cat file.txt            # Read content
cp file.txt backup.txt  # Copy file
mv backup.txt renamed.txt  # Rename
rm renamed.txt          # Delete
cd ..                   # Go up to parent
ls                      # home directory visible
```

**What to Tell Teacher**:
> "I implemented a hierarchical file system with directory support. Files are stored as a linked list of FileNodes, each containing full path, data payload (1KB max), type (file/dir), and size. Path resolution handles absolute paths, relative paths from cwd, and parent directory navigation. Operations include CRUD (create, read, update, delete) plus copy and rename."

---

## 📁 Directory Structure

```
EduOS/
├── boot/                     # Bootloader & kernel entry
│   ├── boot.asm             # Stage 1 bootloader (MBR)
│   ├── kernel_entry.asm     # Entry point (calls kernel_main)
│   ├── gdt.asm              # Global Descriptor Table setup
│   ├── switch_pm.asm        # Real mode → Protected mode
│   ├── disk_load.asm        # Load sectors from disk
│   └── print_string.asm     # Real mode printing
│
├── cpu/                      # CPU-level code
│   ├── idt.c                # Interrupt Descriptor Table
│   ├── idt.h
│   └── interrupt.asm        # Interrupt Service Routines (ISRs)
│
├── drivers/                  # Hardware drivers
│   ├── keyboard.c           # PS/2 keyboard input
│   ├── keyboard.h
│   ├── screen.c             # VGA text mode display
│   ├── screen.h
│   ├── pit.c                # Timer (PIT - Programmable Interval Timer)
│   ├── pit.h
│   ├── ports.c              # I/O port operations
│   └── ports.h
│
├── kernel/                   # Core kernel
│   ├── kernel.c             # Main kernel & command shell
│   ├── types.h              # Standard integer types
│   ├── memory.c             # E820 memory map display
│   ├── memory.h
│   ├── heap.c               # Dynamic memory allocator
│   ├── heap.h
│   ├── paging.c             # Virtual memory (paging)
│   ├── paging.h
│   ├── process.c            # Process management & scheduling
│   ├── process.h
│   ├── fs.c                 # File system
│   ├── fs.h
│   ├── monitor.c            # System monitor utility
│   ├── monitor.h
│   ├── education.c          # Educational OS concepts
│   ├── education.h
│   ├── game.c               # Demo game (Guess the Number)
│   └── game.h
│
├── libc/                     # C library functions
│   ├── string.c             # String manipulation
│   └── string.h
│
├── Makefile                 # Build configuration
├── plan.txt                 # Development notes
└── README.md               # This file
```

---

## 💾 Memory Layout

### Physical Memory

```
0x00000000 - 0x00001000   │ Reserved (BIOS, interrupt vectors)
0x00001000 - 0x00010000   │ Free
0x00010000 - 0x00050000   │ Kernel Code & Data
0x00050000 - 0x00051000   │ E820 Memory Map (from bootloader)
0x00051000 - 0x00100000   │ Free
0x00100000 - 0x00140000   │ Kernel Heap (256 KB)
0x00140000 - 0x00141000   │ Page Directory
0x00141000 - 0x00145000   │ Page Tables (4 KB × 4)
0x00145000 - 0x9FFFFFFF   │ Free (for processes, modules, etc.)
0xB8000    - 0xB8FA0      │ VGA Text Buffer (80×25 chars)
0xFEC00000 - 0xFED00000   │ APIC (advanced interrupt controller)
```

### Virtual Address Space (Per Process)

```
0xFFFFFFFF ┌─────────────────────────────────┐
           │ High Kernel (shared)            │
           │ (kernel code, interrupt stubs)  │
           ├─────────────────────────────────┤
           │                                 │
           │ (Guard page - unmapped)         │
           │                                 │
0x01000000 ├─────────────────────────────────┤
           │ Process User Space:             │
           │ ┌─────────────────────────────┐ │
           │ │ Stack (grows down)          │ │ 4 KB
           │ ├─────────────────────────────┤ │
           │ │ Heap (grows up)             │ │ 256 KB
           │ ├─────────────────────────────┤ │
           │ │ BSS (uninitialized)         │ │
           │ ├─────────────────────────────┤ │
           │ │ Data (initialized globals)  │ │
           │ ├─────────────────────────────┤ │
           │ │ Code (.text)                │ │
           │ └─────────────────────────────┘ │
0x00000000 └─────────────────────────────────┘
```

---

## 🖥️ Command Reference

### Help System
```bash
help              # Show help categories
help files        # File system commands
help system       # System information & control
help process      # Process management
help apps         # Applications & games
```

### File System Commands
```bash
ls                    # List current directory
pwd                   # Print working directory
cd <path>             # Change directory
cd ..                 # Go to parent directory
mkdir <name>          # Create directory
touch <name>          # Create file
cat <name>            # Read file contents
write <file>          # Write to file
rm <name>             # Delete file/directory
cp <src> <dest>       # Copy file
mv <old> <new>        # Rename/move file
```

### System Commands
```bash
echo <text>           # Print text
whoami                # Print user (always "root")
clear                 # Clear screen
reboot                # Restart system
memmap                # Display E820 memory map
uptime                # Show elapsed milliseconds since boot
sleep <ms>            # Sleep for N milliseconds
```

### Process Management
```bash
start                 # Create dummy worker process
kill <pid>            # Terminate process by PID
monitor               # Open system monitor (interactive)
  1                   # Show process list
  2                   # Show memory map
  3                   # Show CPU info
  4 or back           # Return to shell
msg <pid> <char>      # Send 1-byte message to process
inbox <pid>           # Receive message from inbox
```

### Applications
```bash
edu                   # Educational OS concepts tutorial
  A/a                 # Process Scheduling explanation
  B/b                 # Memory Management explanation
  C/c                 # File Systems explanation
  D/d                 # System Calls explanation
  back                # Return to shell

game                  # Play Guess the Number game
  <number 1-100>      # Your guess
  quit                # Exit game
```

---

## 🔨 Build & Run

### Prerequisites
- GCC cross-compiler for i386-elf
- NASM (Netwide Assembler)
- QEMU or VirtualBox (for emulation)
- Make

### Build
```bash
# Compile and link
make

# Output files:
# - os-image        (Disk image, 512 MB)
# - os-image.vdi    (VirtualBox image)
```

### Run on QEMU
```bash
qemu-system-i386 -drive format=raw,file=os-image
```

### Run on VirtualBox
1. Create new VM with Linux (Other) OS type
2. Attach os-image.vdi as storage
3. Boot the VM

### Clean
```bash
make clean
```

---

## 🔍 Technical Details

### Data Structures

#### Process Control Block (PCB)
```c
typedef struct {
    int pid;                 // Process ID
    char name[20];           // Process name
    int state;               // READY/RUNNING/BLOCKED/TERMINATED
    int memory_usage;        // Allocated memory in bytes
    void (*run)();           // Entry function pointer
    int inbox_head, inbox_tail;  // Mailbox cursors
    char inbox[128];         // IPC message buffer
    u32 cr3;                 // Page directory address
    u32 eax, ecx, edx, ebx, esp_save, ebp, esi, edi;  // Saved registers
} Process;
```

#### File Node
```c
typedef struct FileNode {
    char name[32];           // Full path (e.g., "/home/file.txt")
    char data[1024];         // File contents
    int size;                // Content size in bytes
    int type;                // FS_FILE or FS_DIR
    struct FileNode* next;   // Linked list pointer
} FileNode;
```

#### Heap Block Header
```c
typedef struct Block {
    u32 size;                // Payload size in bytes
    u8 free;                 // 1 = free, 0 = in use
    struct Block* next;      // Linked list pointer
} Block;
```

#### IDT Gate
```c
typedef struct {
    u16 low_offset;          // Lower 16 bits of handler address
    u16 sel;                 // Code segment selector
    u8 always0;              // Reserved (must be 0)
    u8 flags;                // Gate type & privilege level
    u16 high_offset;         // Upper 16 bits of handler address
} __attribute__((packed)) idt_gate_t;
```

### Page Table Entry Format

```
31                           12 11    9 8 7 6 5 4 3 2 1 0
┌──────────────────────────────┬────────┬───────────────┐
│     Physical Page Base       │ Avail  │ Flags (0-8)   │
└──────────────────────────────┴────────┴───────────────┘

Flags:
  Bit 0: Present (P)          - Page in physical memory
  Bit 1: Writable (R/W)       - Writable (0=read-only)
  Bit 2: User (U/S)           - User-accessible (0=kernel only)
  Bit 3: Write-Through (WT)   - Write-through caching
  Bit 4: Cache Disable (CD)   - Disable caching
  Bit 5: Accessed (A)         - Page accessed
  Bit 6: Dirty (D)            - Page written to
  Bits 7-8: Available         - Software use
```

### Interrupt Vector Mapping

```
0-31    CPU Exceptions (divide by zero, page fault, etc.)
32      IRQ0 (Timer)          - isr_timer
33      IRQ1 (Keyboard)       - isr_keyboard
34-47   IRQ2-15 (other I/O)   - Reserved
48-255  Software interrupts   - Available
```

### PIC (Programmable Interrupt Controller) Ports

```
Master PIC:
  0x20  - Command register (ICW/OCW)
  0x21  - Interrupt mask register (IMR)

Slave PIC:
  0xA0  - Command register
  0xA1  - Interrupt mask register

EOI (End of Interrupt) command:
  port_byte_out(0x20, 0x20);  - Master PIC EOI
  port_byte_out(0xA0, 0x20);  - Slave PIC EOI
```

---

## 🎓 Presentation Guide for Teacher

### Opening Pitch
> "Professor, I've developed EduOS, a 32-bit x86 educational operating system demonstrating core OS principles. It boots from a custom bootloader, initializes memory management through paging and heap allocation, implements process scheduling and interrupt handling, manages a file system, and provides a functional shell interface—all from scratch in C and x86 assembly."

### Demonstration Sequence

#### 1. Boot Sequence (1 min)
```bash
# Just booted - show initialization
# Point out E820 memory map detection
memmap
```

#### 2. Memory Management (2 min)
```bash
# Virtual memory demonstration
mkdir test
touch file1.txt
touch file2.txt
ls
```

#### 3. Process Management (2 min)
```bash
monitor
1          # Show process table
start      # Create new process
kill 1002  # Terminate process
```

#### 4. Interrupts (2 min)
```bash
uptime     # Timer interrupt (IRQ0)
uptime     # Shows elapsed time
sleep 2000 # Use PIT to sleep 2 seconds
uptime     # Time advanced by 2000+ ms
```

#### 5. File System (3 min)
```bash
pwd
mkdir /home
cd /home
touch myfile.txt
write myfile.txt
cat myfile.txt
cp myfile.txt backup.txt
ls
cd ..
rm /home/backup.txt
```

#### 6. Advanced Features (2 min)
```bash
edu              # Educational module
A                # Show OS concept explanation
back

game             # Demo game
50
game output
quit
```

### Key Points to Emphasize

| Concept | Implementation | Evidence |
|---------|---|---|
| **Paging** | 2-level page tables, identity mapping | memmap command, process isolation |
| **Heap** | Free-list allocator with coalescing | File creation, dynamic allocation |
| **Scheduling** | Round-robin, timer-driven | Process list, new processes |
| **Interrupts** | IDT, PIC remapping, EOI | Keyboard input, timer ticks |
| **I/O Drivers** | VGA, keyboard, PIT | Display, input, timing |
| **File System** | Linked list, directory hierarchy | File operations, path navigation |
| **IPC** | Mailbox messaging | msg/inbox commands |

### Challenges Overcome

1. **Protected Mode Transition**: Setting up GDT and switching from real mode
2. **Interrupt Synchronization**: Preventing race conditions between timer and keyboard
3. **Memory Fragmentation**: Implementing block coalescing in heap
4. **Scancode Complexity**: Handling extended codes for special keys
5. **Path Resolution**: Supporting relative paths, "..", and absolute paths
6. **VGA Cursor Control**: Managing cursor via I/O ports during scrolling

### Closing Statement
> "This project demonstrates end-to-end OS development from bootloader to shell. Every concept from class—paging, scheduling, interrupts, file systems—is implemented in working code. I'm prepared to explain any specific section or demonstrate additional features."

---

## 📊 Features Summary

| Feature | Status | Difficulty | Impact |
|---------|--------|-----------|--------|
| Bootloader | ✅ | High | Core: Without this, nothing runs |
| Protected Mode | ✅ | High | Essential: Required for paging, interrupts |
| Paging | ✅ | High | Memory: Enables isolation, virtual memory |
| Heap Allocator | ✅ | Medium | Memory: Enables dynamic structures |
| IDT/Interrupts | ✅ | High | I/O: Without this, no keyboard/timer |
| Timer (PIT) | ✅ | Medium | Scheduling: Drives process switching |
| Keyboard Driver | ✅ | Medium | I/O: User input, line editing |
| VGA Driver | ✅ | Medium | I/O: User output, scrolling |
| Process Manager | ✅ | High | Core: Multitasking, scheduling |
| File System | ✅ | Medium | Data: Persistent storage (in-memory) |
| IPC | ✅ | Medium | Advanced: Process communication |
| Shell | ✅ | Low | UX: User interaction |

---

## 🚫 Known Limitations

- **No User Mode**: All code runs in kernel mode (no ring 3)
- **No Threading**: Single-threaded processes only
- **No System Calls**: Commands executed directly in kernel
- **No Disk I/O**: File system is in-memory only
- **Limited Process Isolation**: No full memory protection (all processes can see kernel)
- **No Shared Libraries**: Each application bundles needed code
- **256 Processes Max**: Fixed process table size
- **1 MB Pages**: Not implemented (uses only 4 KB pages)

---

## 📖 References

- [Intel IA-32 Architecture Manual](https://www.intel.com/content/www/en/en/architecture-and-technology/64-bit/64-bit-tp.html)
- [OSDev.org](https://osdev.org) - Operating System Development Wiki
- [x86 Memory Paging](https://wiki.osdev.org/Paging)
- [IDT and Interrupt Handling](https://wiki.osdev.org/Interrupt_Descriptor_Table)
- [File Systems](https://wiki.osdev.org/File_Systems)
- [Process Scheduling](https://en.wikipedia.org/wiki/Scheduling_(computing))

---

## 📝 License

Educational project - free to modify and distribute for learning purposes.

---

## ✍️ Author

Developed as an educational demonstration of operating system concepts.

---

**Last Updated**: January 9, 2026

