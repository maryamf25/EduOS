#ifndef FS_H
#define FS_H

#include "types.h"

#define MAX_FILES 20       
#define MAX_FILENAME 32    
#define MAX_FILESIZE 1024

// Flags
#define FS_FILE 0
#define FS_DIR  1

typedef struct {
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    int size;
    int used;
    int type; 
} File;

// --- EXPOSE CWD GLOBALLY ---
extern char cwd[MAX_FILENAME]; 

void init_fs();
void fs_list();
int fs_create(char* name);
int fs_mkdir(char* name);      
int fs_cd(char* path);         
void fs_pwd();                 
int fs_write(char* name, char* data);
void fs_read(char* name);
void fs_delete(char* name);
void fs_copy(char* src, char* dest);
void fs_rename(char* src, char* dest);

#endif