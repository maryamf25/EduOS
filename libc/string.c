#include "string.h"
#include "../kernel/types.h"

// Returns the length of a string
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

// Reverses a string (helper for int_to_ascii)
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// Converts a number to a string (e.g., 123 -> "123")
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

// Adds a character to the end of a string
void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

// Deletes the last character
void backspace(char s[]) {
    int len = strlen(s);
    if (len > 0) s[len-1] = '\0';
}

// Compares two strings. Returns 0 if they are the same.
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}
// libc/string.c (Append to end of file)

void str_delete_at(char s[], int index) {
    int len = strlen(s);
    if (index < 0 || index >= len) return;
    
    // Shift everything left to cover the gap
    for (int i = index; i < len; i++) {
        s[i] = s[i+1];
    }
}

void str_insert_at(char s[], char n, int index) {
    int len = strlen(s);
    // Shift everything right to make space
    for (int i = len; i > index; i--) {
        s[i] = s[i-1];
    }
    s[index] = n;
    s[len + 1] = '\0';
}
int strcasecmp(char s1[], char s2[]) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        char c1 = s1[i];
        char c2 = s2[i];
        
        // Convert to lowercase temporarily for comparison
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        
        if (c1 != c2) return c1 - c2;
        i++;
    }
    
    // Check if one string is longer than the other
    return s1[i] - s2[i];
}
// Returns 1 if string s starts with prefix (case insensitive)
int strcasecmp_prefix(char* s, char* prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        char c1 = s[i];
        char c2 = prefix[i];
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        if (c1 != c2) return 0;
        i++;
    }
    // Must be followed by space or end of string
    if (s[i] != ' ' && s[i] != '\0') return 0;
    return 1;
}
// Copy source to dest
void strcpy(char* dest, char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Concatenate (append) source to dest
void strcat(char* dest, char* src) {
    int i = 0;
    while (dest[i] != '\0') i++; // Find end of dest
    
    int j = 0;
    while (src[j] != '\0') {
        dest[i] = src[j];
        i++; j++;
    }
    dest[i] = '\0';
}

// Check if string starts with prefix
int starts_with(char* str, char* prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (str[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}