#include "heap.h"
#include "types.h"
#include "../drivers/screen.h"

#define HEAP_SIZE (256 * 1024) // 256KB heap

typedef struct Block {
    u32 size;           // payload size in bytes
    u8 free;            // 1=free, 0=used
    struct Block* next;
} Block;

static u8 heap_area[HEAP_SIZE];
static Block* free_list = 0;

static u32 align4(u32 n) { return (n + 3) & ~0x3; }

void init_heap() {
    free_list = (Block*)heap_area;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = 0;
    kprint("[Heap] Init 256KB free-list allocator\n");
}

void split_block(Block* blk, u32 size) {
    // size is already aligned
    if (blk->size >= size + sizeof(Block) + 4) {
        Block* newblk = (Block*)((u8*)blk + sizeof(Block) + size);
        newblk->size = blk->size - size - sizeof(Block);
        newblk->free = 1;
        newblk->next = blk->next;
        blk->size = size;
        blk->next = newblk;
    }
}

void* kmalloc(u32 size) {
    if (size == 0) return 0;
    size = align4(size);
    Block* cur = free_list;
    Block* prev = 0;
    while (cur) {
        if (cur->free && cur->size >= size) {
            split_block(cur, size);
            cur->free = 0;
            return (void*)((u8*)cur + sizeof(Block));
        }
        prev = cur;
        cur = cur->next;
    }
    kprint("[Heap] Out of memory\n");
    return 0;
}

static void coalesce() {
    Block* cur = free_list;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += sizeof(Block) + cur->next->size;
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void kfree(void* ptr) {
    if (!ptr) return;
    Block* blk = (Block*)((u8*)ptr - sizeof(Block));
    blk->free = 1;
    coalesce();
}

