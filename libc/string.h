#ifndef STRING_H
#define STRING_H

void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
void str_delete_at(char s[], int index);
void str_insert_at(char s[], char n, int index);
int strcasecmp(char s1[], char s2[]);
int strcasecmp_prefix(char* s, char* prefix);
void strcpy(char* dest, char* src);
void strcat(char* dest, char* src);
int starts_with(char* str, char* prefix);
#endif