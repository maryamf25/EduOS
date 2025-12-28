#include "fs.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "heap.h"

FileNode* fs_head = 0;
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
    fs_head = 0;
    fs_create("readme.txt");
    fs_write("readme.txt", "Welcome! Root directory.");
    kprint("[FS] File System Initialized.\n");
}

FileNode* fs_find(char* full_path) {
    FileNode* cur = fs_head;
    while (cur) {
        if (strcmp(cur->name, full_path) == 0) return cur;
        cur = cur->next;
    }
    return 0;
}

int fs_create_entry(char* name, int type) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);

    if (fs_find(full_path)) {
        kprint("Error: Name already exists.\n");
        return 0;
    }

    FileNode* node = (FileNode*)kmalloc(sizeof(FileNode));
    if (!node) { kprint("Error: Out of memory.\n"); return 0; }
    strcpy(node->name, full_path);
    node->size = 0;
    node->type = type;
    node->data[0] = '\0';
    node->next = fs_head;
    fs_head = node;
    kprint(type == FS_DIR ? "Directory created.\n" : "File created.\n");
    return 1;
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
    FileNode* d = fs_find(full_path);
    if (d && d->type == FS_DIR) {
        strcpy(cwd, full_path);
        if (cwd[strlen(cwd)-1] != '/') strcat(cwd, "/");
        return 1;
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
    FileNode* cur = fs_head;
    while (cur) {
        if (starts_with(cur->name, cwd)) {
            char* relative = cur->name + strlen(cwd);
            if (strlen(relative) == 0) { cur = cur->next; continue; }

            int is_grandchild = 0;
            for (int j=0; relative[j]!=0; j++) {
                if (relative[j] == '/' && relative[j+1] != 0) is_grandchild = 1;
            }
            if (is_grandchild) { cur = cur->next; continue; }

            if (cur->type == FS_DIR) {
                kprint("[DIR] "); kprint(relative);
            } else {
                kprint("      "); kprint(relative);
            }
            kprint("\n");
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) kprint("(Empty)\n");
}

int fs_write(char* name, char* data) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);
    FileNode* f = fs_find(full_path);
    if (!f) { kprint("Error: File not found.\n"); return 0; }
    if (f->type == FS_DIR) { kprint("Error: Cannot write to directory.\n"); return 0; }
    strcpy(f->data, data);
    f->size = strlen(data);
    kprint("Written.\n");
    return 1;
}

void fs_read(char* name) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);
    FileNode* f = fs_find(full_path);
    if (!f) { kprint("Error: Not found.\n"); return; }
    if (f->type == FS_DIR) { kprint("Error: Is a directory.\n"); return; }
    kprint(f->data); kprint("\n");
}

void fs_delete(char* name) {
    char full_path[MAX_FILENAME];
    get_full_path(name, full_path);
    FileNode* prev = 0; FileNode* cur = fs_head;
    while (cur) {
        if (strcmp(cur->name, full_path) == 0) {
            if (prev) prev->next = cur->next; else fs_head = cur->next;
            kfree(cur);
            kprint("Deleted.\n");
            return;
        }
        prev = cur; cur = cur->next;
    }
    kprint("Error: Not found.\n");
}

void fs_copy(char* src, char* dest) {
    char full_src[MAX_FILENAME]; get_full_path(src, full_src);
    char full_dest[MAX_FILENAME]; get_full_path(dest, full_dest);
    FileNode* src_node = fs_find(full_src);
    if (!src_node) { kprint("Error: Source not found.\n"); return; }

    if (fs_find(full_dest)) { kprint("Error: Dest exists.\n"); return; }

    FileNode* dest_node = (FileNode*)kmalloc(sizeof(FileNode));
    if (!dest_node) { kprint("Error: Out of memory.\n"); return; }
    strcpy(dest_node->name, full_dest);
    dest_node->type = src_node->type;
    dest_node->size = src_node->size;
    strcpy(dest_node->data, src_node->data);
    dest_node->next = fs_head;
    fs_head = dest_node;
    kprint("Copied.\n");
}

void fs_rename(char* src, char* dest) {
    char full_src[MAX_FILENAME]; get_full_path(src, full_src);
    char full_dest[MAX_FILENAME]; get_full_path(dest, full_dest);
    FileNode* f = fs_find(full_src);
    if (!f) { kprint("Error: Source not found.\n"); return; }
    if (fs_find(full_dest)) { kprint("Error: Dest exists.\n"); return; }
    strcpy(f->name, full_dest);
    kprint("Renamed.\n");
}