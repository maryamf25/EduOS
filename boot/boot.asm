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
    call get_memmap      ; 2. Collect BIOS E820 memory map
    call switch_to_pm    ; 3. Switch to 32-bit mode

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

; ------------------------------------------------------------------
; get_memmap: Use BIOS Int 0x15, E820 to read memory map dynamically
; Stores:
;   Count (u32) at physical 0x5000
;   Entries (24 bytes each) starting at 0x5004
; Each entry layout:
;   QWORD base, QWORD length, DWORD type, DWORD ACPI (unused)
; ------------------------------------------------------------------
get_memmap:
    pusha
    ; Point ES:DI to 0x5004 (entries area)
    mov ax, 0x0500        ; ES = 0x0500 -> physical 0x5000
    mov es, ax
    xor di, di
    add di, 4             ; skip count placeholder (0x5000..0x5003)

    ; Zero count
    mov dword [es:0], 0

    xor ebx, ebx          ; continuation value
    mov ecx, 24           ; buffer size
e820_next:
    mov eax, 0xE820
    mov edx, 0x534D4150   ; 'SMAP'
    ; Clear ACPI field (last 4 bytes) as per spec
    mov dword [es:di+20], 0
    int 0x15
    jc e820_done          ; carry -> error, stop
    cmp eax, 0x534D4150
    jne e820_done         ; invalid signature

    ; One entry written to ES:DI
    add di, 24
    ; Increment count
    mov eax, [es:0]
    add eax, 1
    mov [es:0], eax

    test ebx, ebx
    jne e820_next

e820_done:
    popa
    ret

[bits 32]
BEGIN_PM:
    call KERNEL_OFFSET    ; 3. Jump to C Code
    jmp $

BOOT_DRIVE db 0
MSG_LOAD db "Loading EduOS Kernel...", 0

times 510-($-$$) db 0
dw 0xaa55