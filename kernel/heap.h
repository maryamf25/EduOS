#ifndef HEAP_H
#define HEAP_H

#include "types.h"

void init_heap();
void* kmalloc(u32 size);
void kfree(void* ptr);

#endif
