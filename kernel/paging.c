#include "paging.h"

static u32 kernel_pd[1024] __attribute__((aligned(4096)));
static u32 kernel_pts[4][1024] __attribute__((aligned(4096)));

// Simple frame allocator: 64 frames (64*4KB = 256KB) for demo
static u8 frame_bitmap[64];

u32 alloc_frame() {
    for (int i=0; i<64; i++) {
        if (frame_bitmap[i] == 0) { frame_bitmap[i] = 1; return i * 0x1000; }
    }
    return 0; // out of frames
}

static void identity_map_first_16mb(u32 *pd) {
    for (int i=0; i<4; i++) {
        u32 *pt = kernel_pts[i];
        for (int j=0; j<1024; j++) {
            pt[j] = (i*1024 + j)*0x1000 | 3; // present, writable
        }
        pd[i] = ((u32)pt) | 3; // present, writable
    }
}

void init_paging() {
    // Identity map first 16MB in kernel PD
    for (int i=0; i<1024; i++) kernel_pd[i] = 0;
    identity_map_first_16mb(kernel_pd);

    // Load CR3 with kernel PD
    u32 pd_phys = (u32)kernel_pd;
    __asm__ volatile("mov %0, %%cr3" : : "r" (pd_phys));

    // Enable paging (CR0.PG=1)
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

page_dir_t create_address_space() {
    // For demo, clone kernel PD identity mapping
    u32 *pd = (u32*)alloc_frame();
    if (pd == 0) return 0;

    // Align and copy entries (we only use first 4 entries here)
    for (int i=0; i<1024; i++) pd[i] = 0;
    // Reuse kernel page tables for identity mapping (shared)
    for (int i=0; i<4; i++) pd[i] = ((u32)kernel_pts[i]) | 3;

    return (page_dir_t)pd;
}

void switch_address_space(page_dir_t pd_phys) {
    if (pd_phys) {
        __asm__ volatile("mov %0, %%cr3" : : "r" (pd_phys));
    }
}

void map_page(page_dir_t pd_phys, u32 virt, u32 phys, int writable) {
    // Minimal: assumes virt < 16MB (first four entries)
    u32 dir_index = virt >> 22;
    u32 tbl_index = (virt >> 12) & 0x3FF;
    u32 *pd = (u32*)pd_phys;
    u32 *pt = (u32*)(pd[dir_index] & ~0xFFF);
    pt[tbl_index] = (phys & ~0xFFF) | (writable?3:1);
}
