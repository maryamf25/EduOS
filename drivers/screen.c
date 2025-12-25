#include "screen.h"
#include "ports.h"
#include "../kernel/types.h" // Needed for memory copy

// Private helper functions
int get_offset(int col, int row) { 
    return 2 * (row * MAX_COLS + col); 
}

int get_offset_row(int offset) { 
    return offset / (2 * MAX_COLS); 
}

int get_offset_col(int offset) { 
    return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2; 
}

int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

// --- NEW FUNCTION: Memory Copy Helper ---
void memory_copy(char *source, char *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

// --- NEW FUNCTION: Handle Scrolling ---
int handle_scrolling(int cursor_offset) {
    // If the cursor is within the screen, return it unmodified
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }

    // Shuffle the rows back one
    int i;
    for (i = 1; i < MAX_ROWS; i++) {
        memory_copy(
            (char*)(get_offset(0, i) + VIDEO_ADDRESS),
            (char*)(get_offset(0, i-1) + VIDEO_ADDRESS),
            MAX_COLS * 2
        );
    }

    // Clear the last line (blank it out)
    char* last_line = (char*)(get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS);
    for (i = 0; i < MAX_COLS * 2; i++) {
        last_line[i] = 0;
    }

    // Move cursor to the start of the last line
    cursor_offset -= 2 * MAX_COLS;
    return cursor_offset;
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    char *screen = (char *)VIDEO_ADDRESS;
    for (int i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

void kprint_at(char *message, int col, int row) {
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;
    while (message[i] != 0) {
        offset = handle_scrolling(offset); // <--- Check for scroll!
        
        if (message[i] == '\n') {
            row = get_offset_row(offset);
            offset = get_offset(0, row + 1);
        } else {
            char *vidmem = (char *)VIDEO_ADDRESS;
            vidmem[offset] = message[i];
            vidmem[offset+1] = WHITE_ON_BLACK;
            offset += 2;
        }
        i++;
    }
    offset = handle_scrolling(offset); // <--- Check final scroll
    set_cursor_offset(offset);
}

void kprint(char *message) {
    kprint_at(message, -1, -1);
}

void kprint_backspace() {
    int offset = get_cursor_offset() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    char *vidmem = (char *)VIDEO_ADDRESS;
    vidmem[offset] = ' ';
    vidmem[offset+1] = WHITE_ON_BLACK;
    set_cursor_offset(offset);
}

void kprint_move_cursor(int direction) {
    int offset = get_cursor_offset();
    offset += (direction * 2);
    if (offset < 0) offset = 0;
    // Don't let it go off screen
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        offset = MAX_ROWS * MAX_COLS * 2 - 2;
    }
    set_cursor_offset(offset);
}