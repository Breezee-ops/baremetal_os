#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"


void init_paging() {
    int i;

    // set page directory to not present 4MB chunks
    for(i = 0; i < 1024; i++) {
        page_directory[i].present = 0;
        page_directory[i].read_write = 1;
        page_directory[i].user_supervisor = 0;
        page_directory[i].write_through = 0;
        page_directory[i].cache_disable = 0;
        page_directory[i].accessed = 0;
        page_directory[i].reserved = 0;
        page_directory[i].page_size = 1;
        page_directory[i].not_used = 0;
        page_directory[i].available = 0;
        page_directory[i].PT_addr = i << 10;
    }

    //set first 4MB to "not present" chunks of 4kB
    page_directory[0].present = 1;
    page_directory[0].read_write = 1;
    page_directory[0].user_supervisor = 0;
    page_directory[0].write_through = 0;
    page_directory[0].cache_disable = 0;
    page_directory[0].accessed = 0;
    page_directory[0].reserved = 0;
    page_directory[0].page_size = 0;
    page_directory[0].not_used = 0;
    page_directory[0].available = 0;
    page_directory[0].PT_addr = (unsigned int)page_table >> 12; // flag

    //set next 4 MB to kernel
    page_directory[1].present = 1;
    page_directory[1].read_write = 1;
    page_directory[1].user_supervisor = 0;
    page_directory[1].write_through = 0;
    page_directory[1].cache_disable = 0;
    page_directory[1].accessed = 0;
    page_directory[1].reserved = 0;
    page_directory[1].page_size = 1;
    page_directory[1].not_used = 0;
    page_directory[1].available = 0;
    page_directory[1].PT_addr = 0x1 << 10; //1 * 2^10 * 2^12 = 2^22 = 4MB start of kernel

    
    //set page table of first 4 MB
    for(i = 0; i < 1024; i++) {
        page_table[i].present = 0;
        page_table[i].read_write = 1;
        page_table[i].user_supervisor = 0;
        page_table[i].write_through = 0;
        page_table[i].cache_disable = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].reserved = 0;
        page_table[i].global = 0;
        page_table[i].available = 0;
        page_table[i].P_addr = i;
    }

    //set page table index of video memory
    page_table[VIDEO_MEMORY].present = 1;
    page_table[VIDEO_MEMORY].read_write = 1;
    page_table[VIDEO_MEMORY].user_supervisor = 0;
    page_table[VIDEO_MEMORY].write_through = 0;
    page_table[VIDEO_MEMORY].cache_disable = 0;
    page_table[VIDEO_MEMORY].accessed = 0;
    page_table[VIDEO_MEMORY].dirty = 0;
    page_table[VIDEO_MEMORY].reserved = 0;
    page_table[VIDEO_MEMORY].global = 0;
    page_table[VIDEO_MEMORY].available = 0;
    page_table[VIDEO_MEMORY].P_addr = VIDEO_MEMORY;

    enable_paging();
    // for(i = 0; i < 1024; i++)
    // {
    //     // // sets the following flags to the pages:
    //     // // supervisor mode
    //     // // write enabled
    //     // // not present
    //     page_directory[i] = (i * 0x400000);

    //     // set entire page_table to not present
    //     page_table[i] = (i * 0x1000) | 0x2;
    // }
    // //set video memory to present
    // page_table[VIDEO_MEMORY] = (VIDEO_MEMORY * 0x1000) | 0x07;

    // //4kB mode
    // page_directory[0] = ((unsigned int)page_table * 0x1000) | 0x3;//add first 4MB of mem to PD
    // //4MB mode
    // // 0000 0000 01|00 0000 000|0| 000|1| 1|0|0|0| 0|0|1|1|
    // // 1            2           3  4   5  6 7 8 9 10 11 12 13
    // //
    // // 1: Page base address(10) = 4MB
    // // 2: Reserved(9)
    // // 3: Page Attribute Table
    // // 4: Available(3)
    // // 5: PS = 4MB
    // // 6: dirty
    // // 7: Accessed
    // // 8: Cache Disable
    // // 9: Write-Through
    // //10: User/Supervisor
    // //11: Read/Write
    // //12: Present
    // page_directory[1] = 0x00400183;


    
}

void enable_paging() {
    asm (
    "movl $page_directory, %%eax      ;"
    "movl %%eax, %%cr3                ;"

    "movl %%cr4, %%eax                ;"
    "orl $0x00000010, %%eax           ;"
    "movl %%eax, %%cr4                ;"

    "movl %%cr0, %%eax                ;"
    "orl $0x80000000, %%eax           ;"
    "movl %%eax, %%cr0                 "

    : : : "eax", "cc" );
}
