#ifndef PAGING_H
#define PAGING_H

#include "types.h"

typedef u32 page_dir_t;

void init_paging();
page_dir_t create_address_space();
void switch_address_space(page_dir_t pd_phys);
u32 alloc_frame();
void map_page(page_dir_t pd_phys, u32 virt, u32 phys, int writable);

#endif
