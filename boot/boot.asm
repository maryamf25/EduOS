[org 0x7c00]
KERNEL_OFFSET equ 0x1000 

    xor ax, ax      
    mov ds, ax      
    mov es, ax      
    mov ss, ax
    mov bp, 0x9000
    mov sp, bp

    mov [BOOT_DRIVE], dl ; Remember the drive number!

    call load_kernel     ; 1. Load C kernel from disk
    call switch_to_pm    ; 2. Switch to 32-bit mode

    jmp $                ; Freeze if we fail

; Include the helpers
%include "boot/print_string.asm"
%include "boot/disk_load.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD
    call print_string
    
    mov bx, KERNEL_OFFSET ; Load to 0x1000
    mov dh, 50            ; Read 15 sectors
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    call KERNEL_OFFSET    ; 3. Jump to C Code
    jmp $

BOOT_DRIVE db 0
MSG_LOAD db "Loading EduOS Kernel...", 0

times 510-($-$$) db 0
dw 0xaa55