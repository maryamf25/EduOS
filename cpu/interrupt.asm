[global isr_keyboard]
[extern isr_keyboard_handler]

[global isr_timer]
[extern isr_timer_handler]

isr_keyboard:
    pusha           ; Save registers
    call isr_keyboard_handler
    popa            ; Restore registers
    iret            ; Return from interrupt

isr_timer:
    pusha
    mov eax, esp      ; pass pointer to saved regs
    push eax
    call isr_timer_handler
    add esp, 4
    popa
    iret