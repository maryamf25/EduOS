[global isr_keyboard]
[extern isr_keyboard_handler]

isr_keyboard:
    pusha           ; Save registers
    call isr_keyboard_handler
    popa            ; Restore registers
    iret            ; Return from interrupt