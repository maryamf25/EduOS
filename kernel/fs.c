#include "fs.h"
#include "../libc/string.h"
#include "../drivers/screen.h"

File file_system[MAX_FILES];
char cwd[MAX_FILENAME] = "/"; // Global Current Working Directory

// Helper: Resolve full path (e.g., "file.txt" -> "/home/file.txt")
void get_full_path(char* name, char* full_path) {
    if (name[0] == '/') {
        strcpy(full_path, name);
    } else {
        strcpy(full_path, cwd);
        int len = strlen(full_path);
        if (len > 0 && full_path[len-1] != '/') {
            strcat(full_path, "/");
        }
        strcat(full_path, name);
    }
}

void init_fs() {
    for (int i = 0; i < MAX_FILES; i++) file_system[i].used = 0;
    fs_create("readme.txt");
    fs_write("readme.txt", "Welcome! Root directory.");
    kprint("[FS] File System Initialized.\n");
}

int fs_create_entry(char* name, int type) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);

    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_path) == 0) {
            kprint("Error: Name already exists.\n");
            return 0;
        }
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used == 0) {
            file_system[i].used = 1;
            strcpy(file_system[i].name, full_path);
            file_system[i].size = 0;
            file_system[i].type = type;
            kprint(type == FS_DIR ? "Directory created.\n" : "File created.\n");
            return 1;
        }
    }
    kprint("Error: Disk full.\n");
    return 0;
}

int fs_create(char* name) { return fs_create_entry(name, FS_FILE); }
int fs_mkdir(char* name) { return fs_create_entry(name, FS_DIR); }

int fs_cd(char* path) {
    // 1. Handle Root
    if (strcmp(path, "/") == 0) {
        strcpy(cwd, "/");
        return 1;
    }
    
    // 2. Handle ".." (Go Back)
    if (strcmp(path, "..") == 0) {
        if (strcmp(cwd, "/") == 0) return 0; // Already at root, do nothing
        
        int len = strlen(cwd);
        // If cwd is "/home/", len is 6. We want to remove 'home/'
        
        // Step A: Ignore the very last slash if it exists
        if (len > 1 && cwd[len-1] == '/') len--;
        
        // Step B: Walk backwards until we hit the next slash
        while (len > 1 && cwd[len-1] != '/') {
            len--;
        }
        
        // Step C: Cut the string there
        cwd[len] = '\0';
        // If we reduced it to empty, it means root
        if (strlen(cwd) == 0) strcpy(cwd, "/");
        
        return 1;
    }

    char full_path[MAX_FILENAME];
    get_full_path(path, full_path);

    // 3. Verify Directory Exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && file_system[i].type == FS_DIR && strcmp(file_system[i].name, full_path) == 0) {
            strcpy(cwd, full_path);
            // Ensure trailing slash for consistency
            if (cwd[strlen(cwd)-1] != '/') strcat(cwd, "/");
            return 1;
        }
    }
    kprint("Error: Directory not found.\n");
    return 0;
}

void fs_pwd() {
    kprint(cwd);
    kprint("\n");
}

void fs_list() {
    kprint("Listing: "); kprint(cwd); kprint("\n");
    int found = 0;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used) {
            if (starts_with(file_system[i].name, cwd)) {
                char* relative = file_system[i].name + strlen(cwd);
                if (strlen(relative) == 0) continue; 
                
                int is_grandchild = 0;
                for (int j=0; relative[j]!=0; j++) {
                    if (relative[j] == '/' && relative[j+1] != 0) is_grandchild = 1;
                }
                if (is_grandchild) continue;

                if (file_system[i].type == FS_DIR) {
                    kprint("[DIR] "); kprint(relative);
                } else {
                    kprint("      "); kprint(relative);
                }
                kprint("\n");
                found = 1;
            }
        }
    }
    if (!found) kprint("(Empty)\n");
}

int fs_write(char* name, char* data) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);

    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_path) == 0) {
            if (file_system[i].type == FS_DIR) {
                kprint("Error: Cannot write to directory.\n");
                return 0;
            }
            strcpy(file_system[i].data, data);
            file_system[i].size = strlen(data);
            kprint("Written.\n");
            return 1;
        }
    }
    kprint("Error: File not found.\n");
    return 0;
}

void fs_read(char* name) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);

    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_path) == 0) {
            if (file_system[i].type == FS_DIR) kprint("Error: Is a directory.\n");
            else { kprint(file_system[i].data); kprint("\n"); }
            return;
        }
    }
    kprint("Error: Not found.\n");
}

void fs_delete(char* name) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_path) == 0) {
            file_system[i].used = 0;
            kprint("Deleted.\n");
            return;
        }
    }
    kprint("Error: Not found.\n");
}

void fs_copy(char* src, char* dest) {
    char full_src[MAX_FILENAME]; get_full_path(src, full_src);
    char full_dest[MAX_FILENAME]; get_full_path(dest, full_dest);

    int src_idx = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_src) == 0) {
            src_idx = i; break;
        }
    }
    if (src_idx == -1) { kprint("Error: Source not found.\n"); return; }

    int dest_idx = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_system[i].used) { dest_idx = i; break; }
    }
    if (dest_idx == -1) { kprint("Error: Disk full.\n"); return; }

    file_system[dest_idx].used = 1;
    strcpy(file_system[dest_idx].name, full_dest);
    file_system[dest_idx].type = file_system[src_idx].type;
    file_system[dest_idx].size = file_system[src_idx].size;
    strcpy(file_system[dest_idx].data, file_system[src_idx].data);
    kprint("Copied.\n");
}

void fs_rename(char* src, char* dest) {
    char full_src[MAX_FILENAME]; get_full_path(src, full_src);
    char full_dest[MAX_FILENAME]; get_full_path(dest, full_dest);

    for (int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].used && strcmp(file_system[i].name, full_src) == 0) {
            strcpy(file_system[i].name, full_dest);
            kprint("Renamed.\n");
            return;
        }
    }
    kprint("Error: Source not found.\n");
}