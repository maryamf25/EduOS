# EduOS - Educational Operating System

A minimal, educational operating system built from scratch to demonstrate core OS concepts including bootloading, memory management, process management, file systems, and device drivers.

![Version](https://img.shields.io/badge/version-1.2-blue)
![License](https://img.shields.io/badge/license-Educational-green)

## 📋 Overview

EduOS is a 32-bit x86 operating system developed as an educational project to understand fundamental operating system concepts. It features a simple command-line interface with basic file system operations, process management, and keyboard interaction.

## ✨ Features

### Core OS Components
- **Custom Bootloader**: Written in x86 assembly (NASM)
- **Protected Mode**: Transitions from 16-bit real mode to 32-bit protected mode
- **GDT (Global Descriptor Table)**: Memory segmentation setup
- **IDT (Interrupt Descriptor Table)**: Interrupt handling framework
- **Kernel**: Core kernel written in C

### File System
- In-memory file system with directory support
- Hierarchical directory structure
- File operations: create, read, write, delete, copy, rename
- Directory operations: mkdir, cd, pwd, ls
- Current working directory tracking

### Process Management
- Basic process creation and tracking
- Process state management (READY, RUNNING, BLOCKED)
- Process listing with PID, state, memory usage, and name
- Task manager interface

### Device Drivers
- **Keyboard Driver**: PS/2 keyboard input with scancode translation
- **Screen Driver**: VGA text mode output (80x25)
- **Port I/O**: Low-level hardware communication

### System Commands
- `help` - Display all available commands
- `clear` - Clear the screen
- `echo [text]` - Print text to screen
- `whoami` - Display current user
- `reboot` - Restart the system

## 🛠️ Project Structure

```
EduOS/
├── boot/               # Bootloader and boot-related code
│   ├── boot.asm       # Main bootloader
│   ├── disk_load.asm  # Disk reading routines
│   ├── gdt.asm        # Global Descriptor Table setup
│   ├── kernel_entry.asm
│   ├── print_string.asm
│   └── switch_pm.asm  # Protected mode switch
├── cpu/               # CPU-related functionality
│   ├── idt.c/h        # Interrupt Descriptor Table
│   └── interrupt.asm  # Interrupt handlers
├── drivers/           # Hardware drivers
│   ├── keyboard.c/h   # Keyboard driver
│   ├── ports.c/h      # I/O port operations
│   └── screen.c/h     # VGA text mode driver
├── kernel/            # Kernel core
│   ├── kernel.c       # Main kernel logic
│   ├── fs.c/h         # File system implementation
│   ├── process.c/h    # Process management
│   └── types.h        # Type definitions
├── libc/              # Standard library functions
│   ├── string.c/h     # String manipulation
└── makefile           # Build configuration
```

## 🚀 Getting Started

### Prerequisites

- **NASM** (Netwide Assembler)
- **GCC** (GNU Compiler Collection) with 32-bit support
- **LD** (GNU Linker)
- **QEMU** (Quick Emulator) for testing

#### On Ubuntu/Debian:
```bash
sudo apt-get install build-essential nasm qemu-system-x86
sudo apt-get install gcc-multilib g++-multilib
```

#### On Arch Linux:
```bash
sudo pacman -S base-devel nasm qemu
```

### Building EduOS

1. Clone the repository:
```bash
cd EduOS
```

2. Build the OS image:
```bash
make
```

This will compile all sources and create an `os-image` bootable binary.

3. Run in QEMU:
```bash
make run
```

Or manually:
```bash
qemu-system-i386 -fda os-image
```

4. Clean build artifacts:
```bash
make clean
```

## 📖 Usage Guide

### File System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `ls` | List files in current directory | `ls` |
| `pwd` | Print working directory | `pwd` |
| `cd [path]` | Change directory | `cd /home` or `cd ..` |
| `mkdir [name]` | Create directory | `mkdir mydir` |
| `touch [name]` | Create file | `touch file.txt` |
| `cat [name]` | Display file contents | `cat readme.txt` |
| `write [name]` | Write data to file | `write file.txt` |
| `rm [name]` | Delete file | `rm file.txt` |
| `cp [src] [dst]` | Copy file | `cp file1.txt file2.txt` |
| `mv [old] [new]` | Rename/move file | `mv old.txt new.txt` |

### Process Management Commands

| Command | Description |
|---------|-------------|
| `monitor` | Display task manager (list all processes) |
| `start` | Create a new dummy process |

### System Commands

| Command | Description |
|---------|-------------|
| `help` | Display help menu |
| `clear` | Clear the screen |
| `echo [text]` | Print text to console |
| `whoami` | Show current user (root) |
| `reboot` | Restart the system |

### Example Session

```
EduOS Kernel v1.2
Type 'help' for commands.

root@EduOS:/$ mkdir home
Directory created.
root@EduOS:/$ cd home
root@EduOS:/home/$ touch myfile.txt
File created.
root@EduOS:/home/$ write myfile.txt
File written.
root@EduOS:/home/$ cat myfile.txt
Data written by user
root@EduOS:/home/$ ls
myfile.txt
root@EduOS:/home/$ pwd
/home/
root@EduOS:/home/$ monitor

PID   | STATE | MEMORY | NAME
----------------------------------
1000  | RUNNING | 4096 | KERNEL
1001  | RUNNING | 2048 | SHELL
```

## 🏗️ Technical Details

### Boot Process
1. BIOS loads the bootloader from the first sector (boot.asm)
2. Bootloader loads additional sectors from disk
3. Sets up GDT for protected mode
4. Switches to 32-bit protected mode
5. Jumps to kernel entry point
6. Kernel initializes IDT, drivers, and subsystems

### Memory Layout
- Bootloader: 0x7C00 (BIOS loads here)
- Kernel: 0x1000 (loaded by bootloader)
- VGA Text Buffer: 0xB8000

### Architecture
- **Target**: x86 (32-bit)
- **Assembler**: NASM
- **Compiler**: GCC with `-m32 -ffreestanding`
- **Linker**: GNU LD with custom text base at 0x1000

## 🎓 Learning Objectives

This project demonstrates:
- Low-level x86 assembly programming
- Bootloader development
- Protected mode and memory segmentation
- Interrupt handling and IDT setup
- Device driver development (keyboard, VGA)
- Basic file system implementation
- Process management fundamentals
- Systems programming in C

## 🔧 Development

### Adding New Features

1. **New Command**: Add handling in `kernel/kernel.c` in the `user_input()` function
2. **New Driver**: Create files in `drivers/` and initialize in `kernel_main()`
3. **File System**: Extend functionality in `kernel/fs.c`
4. **Process Management**: Modify `kernel/process.c`

### Debugging

Use QEMU with GDB for debugging:
```bash
qemu-system-i386 -fda os-image -s -S &
gdb -ex "target remote localhost:1234" -ex "break *0x1000"
```

## ⚠️ Limitations

- Single-threaded (no multitasking/scheduling)
- No dynamic memory allocation (heap)
- In-memory file system (no persistence)
- Limited to 64 files
- No user-space separation
- Basic error handling
- Fixed 80x25 VGA text mode only

## 🤝 Contributing

This is an educational project. Feel free to fork and experiment!

## 📝 License

This project is for educational purposes. Feel free to use and modify for learning.

## 👨‍💻 Author

Created as a semester 5 Operating Systems course project.


**Note**: This is a learning project and not intended for production use. It demonstrates fundamental OS concepts in a simplified environment.
