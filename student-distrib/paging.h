#include "types.h"
#ifndef _PAGING_H
#define _PAGING_H
#ifndef ASM
#define VIDEO_MEMORY 0xb8

typedef union page_directory_t {
    uint32_t val;
    struct {
        uint32_t present : 1;
        uint32_t read_write : 1;
        uint32_t user_supervisor : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t reserved : 1;
        uint32_t page_size : 1;
        uint32_t not_used : 1;
        uint32_t available : 3;// flag
        uint32_t PT_addr : 20;
    } __attribute__ ((packed));
   
} page_directory_t;
typedef union page_table_t {
    uint32_t val;
    struct {
        uint32_t present : 1;
        uint32_t read_write : 1;
        uint32_t user_supervisor : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t dirty : 1;
        uint32_t reserved : 1;
        uint32_t global : 1;
        uint32_t available : 3;// flag
        uint32_t P_addr : 20;
    } __attribute__ ((packed));
} page_table_t;
page_directory_t page_directory[1024] __attribute__((aligned(4096)));// 1024 entries aligned to 4kB
page_table_t page_table[1024] __attribute__((aligned(4096)));// 1024 entries aligned to 4kB
page_table_t page_vidmap[1024] __attribute__((aligned(4096)));// 1024 entries aligned to 4kB

volatile char* curmem;

void init_paging();
void enable_paging();
void set_exe_page(uint32_t pid);
void flush_tlb(); 
void mod_vidmem(int term);
void from_buf(int term);
void to_buf(int term);
void write_to_buf(int term);
void page_vidmap_sched(int term);
// char* curmem;

#endif /* ASM */
#endif /* _PAGING_H */
