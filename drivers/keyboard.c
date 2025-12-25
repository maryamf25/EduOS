#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include "../cpu/idt.h"
#include "../libc/string.h"

#define SC_MAX 57
#define BACKSPACE 0x0E
#define ENTER 0x1C

// Control Keys
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define CAPS_LOCK 0x3A
#define L_SHIFT_REL 0xAA
#define R_SHIFT_REL 0xB6

// Extended Keys
#define ARROW_UP    0x48
#define ARROW_DOWN  0x50
#define ARROW_LEFT  0x4B
#define ARROW_RIGHT 0x4D
#define DELETE_KEY  0x53

static char key_buffer[256]; 
static int extended_mode = 0; 
static int cursor_pos = 0; 

// State tracking
static int shift_pressed = 0; 
static int caps_lock_active = 0; // <--- NEW: Track Caps Lock

// Lowercase by default
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

void user_input(char *input);

void redraw_line_tail() {
    int offset_backup = get_cursor_offset();
    kprint(&key_buffer[cursor_pos]);
    kprint(" ");
    set_cursor_offset(offset_backup);
}

void isr_keyboard_handler() {
    u8 scancode = port_byte_in(0x60);
    port_byte_out(0x20, 0x20);

    // 1. Handle Shift Press/Release
    if (scancode == L_SHIFT || scancode == R_SHIFT) {
        shift_pressed = 1;
        return;
    }
    if (scancode == L_SHIFT_REL || scancode == R_SHIFT_REL) {
        shift_pressed = 0;
        return;
    }

    // 2. Handle Caps Lock (Toggle on press)
    if (scancode == CAPS_LOCK) {
        caps_lock_active = !caps_lock_active; // Flip state (0->1 or 1->0)
        return;
    }

    if (scancode == 0xE0) {
        extended_mode = 1;
        return;
    }

    if (extended_mode) {
        extended_mode = 0;
        int len = strlen(key_buffer);
        switch (scancode) {
            case ARROW_LEFT:
                if (cursor_pos > 0) {
                    cursor_pos--;
                    kprint_move_cursor(-1);
                }
                break;
            case ARROW_RIGHT:
                if (cursor_pos < len) {
                    cursor_pos++;
                    kprint_move_cursor(1);
                }
                break;
            case DELETE_KEY:
                if (cursor_pos < len) {
                    str_delete_at(key_buffer, cursor_pos);
                    redraw_line_tail();
                }
                break;
        }
        return;
    }

    if (scancode > SC_MAX) return;

    if (scancode == BACKSPACE) {
        if (cursor_pos > 0) {
            str_delete_at(key_buffer, cursor_pos - 1);
            cursor_pos--;
            kprint_backspace();
            redraw_line_tail();
        }
    } 
    else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer);
        key_buffer[0] = '\0';
        cursor_pos = 0;
    } 
    else {
        char letter = sc_ascii[(int)scancode];
        
        // 3. Determine if Uppercase is needed
        // Uppercase if: (Shift IS pressed) XOR (Caps Lock IS active)
        // This means:
        // Shift=0, Caps=0 -> Lower
        // Shift=1, Caps=0 -> Upper
        // Shift=0, Caps=1 -> Upper
        // Shift=1, Caps=1 -> Lower (Standard behavior)
        int uppercase = (shift_pressed ^ caps_lock_active);

        if (uppercase) {
            if (letter >= 'a' && letter <= 'z') {
                letter -= 32; // Convert to Uppercase
            }
            // Note: Caps Lock usually doesn't affect numbers/symbols,
            // so we only apply symbol shift if SHIFT is actually held.
            if (shift_pressed) {
                if (letter == '1') letter = '!';
                else if (letter == '9') letter = '(';
                else if (letter == '0') letter = ')';
                else if (letter == '-') letter = '_';
                else if (letter == '=') letter = '+';
            }
        }

        str_insert_at(key_buffer, letter, cursor_pos);
        
        char str[2] = {letter, '\0'};
        kprint(str);
        
        cursor_pos++;
        if (cursor_pos < strlen(key_buffer)) {
             redraw_line_tail();
        }
    }
}

void init_keyboard() {
    port_byte_out(0x20, 0x11); port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20); port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04); port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01); port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0xFD); port_byte_out(0xA1, 0xFF);
    
    extern void isr_keyboard();
    set_idt_gate(33, (u32)isr_keyboard);
    set_idt();
    __asm__ volatile("sti");
}