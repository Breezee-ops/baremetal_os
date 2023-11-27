#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
/* init_paging - Example
 * 
 * goes through the paging directory and sets all accesible addresses to available. Also initializes each entry in page directory
 * Inputs: None
 * Outputs: initialised page_directory and page table so that they are seen a spresent and carrying the corresponding address
 * Side Effects: changes values in page_directory and page_table
 * Coverage: page_directory and page_table def.
 */
char* curmem;
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

    page_directory[32].present = 1;
    page_directory[32].read_write = 1;
    page_directory[32].user_supervisor = 1;
    page_directory[32].write_through = 0;
    page_directory[32].cache_disable = 0;
    page_directory[32].accessed = 0;
    page_directory[32].reserved = 0;
    page_directory[32].page_size = 1;
    page_directory[32].not_used = 0;
    page_directory[32].available = 0;
    page_directory[32].PT_addr = 0x2 << 10; // 8 mb

    //set page table index of video memory
    for(i = 0; i < 3; i ++){
        page_table[VIDEO_MEMORY + i].present = 1;
        page_table[VIDEO_MEMORY + i].read_write = 1;
        page_table[VIDEO_MEMORY + i].user_supervisor = 0;//testing
        page_table[VIDEO_MEMORY + i].write_through = 0;
        page_table[VIDEO_MEMORY + i].cache_disable = 0;
        page_table[VIDEO_MEMORY + i].accessed = 0;
        page_table[VIDEO_MEMORY + i].dirty = 0;
        page_table[VIDEO_MEMORY + i].reserved = 0;
        page_table[VIDEO_MEMORY + i].global = 0;
        page_table[VIDEO_MEMORY + i].available = 0;
        page_table[VIDEO_MEMORY + i].P_addr = VIDEO_MEMORY + i;
    }
    page_directory[33].present = 1;
    page_directory[33].read_write = 1;
    page_directory[33].user_supervisor = 1;
    page_directory[33].write_through = 0;
    page_directory[33].cache_disable = 0;
    page_directory[33].accessed = 0;
    page_directory[33].reserved = 0;
    page_directory[33].page_size = 0;
    page_directory[33].not_used = 0;
    page_directory[33].available = 0;
    page_directory[33].PT_addr = ((unsigned int)page_vidmap) >> 12; // 8 mb

    for(i = 0; i < 1024; i++) {
        page_vidmap[i].present = 0;
        page_vidmap[i].read_write = 1;
        page_vidmap[i].user_supervisor = 1;
        page_vidmap[i].write_through = 0;
        page_vidmap[i].cache_disable = 0;
        page_vidmap[i].accessed = 0;
        page_vidmap[i].dirty = 0;
        page_vidmap[i].reserved = 0;
        page_vidmap[i].global = 0;
        page_vidmap[i].available = 0;
        page_vidmap[i].P_addr = i;
    }
    page_vidmap[0].present = 1;
    page_vidmap[0].read_write = 1;
    page_vidmap[0].user_supervisor = 1;
    page_vidmap[0].write_through = 0;
    page_vidmap[0].cache_disable = 0;
    page_vidmap[0].accessed = 0;
    page_vidmap[0].dirty = 0;
    page_vidmap[0].reserved = 0;
    page_vidmap[0].global = 0;
    page_vidmap[0].available = 0;
    page_vidmap[0].P_addr = VIDEO_MEMORY;

    curmem = 0xB8000;

    enable_paging();
}
/* enable_paging
 * 
 * sets the paging directory registers so that they carry the right address for the pages. Kernel address, video memory address and the address of the actual page directory
 * is loaded into the registers here
 * Inputs: None
 * Outputs: eax
 * Side Effects: Sets the registers for paging
 * Coverage: eax, cr0, cr3, cr4
 */
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

void flush_tlb(){
    asm (
    "movl $page_directory, %%eax      ;"
    "movl %%eax, %%cr3                ;"
    : : : "eax", "cc" );
}

void set_exe_page(uint32_t pid){
    // for 8mb + (processnum*4mb)
    // 0x2 << 10 + (processnum * 0x1 << 10)
    page_directory[32].present = 1;
    page_directory[32].read_write = 1;
    page_directory[32].user_supervisor = 1;
    page_directory[32].write_through = 0;
    page_directory[32].cache_disable = 0;
    page_directory[32].accessed = 0;
    page_directory[32].reserved = 0;
    page_directory[32].page_size = 1;
    page_directory[32].not_used = 0;
    page_directory[32].available = 0;
    page_directory[32].PT_addr = (2 + (pid)) << 10; //0x2 << 10 + (processnum * 0x1 << 10) start of process memory.

    //enable_paging();

    flush_tlb();
}

void to_buf(int term){
    memcpy((unsigned char*)((VIDEO_MEMORY + term)<<12), (VIDEO_MEMORY<<12), 4096);
}

void write_to_buf(int term){
    curmem = (unsigned char*)((VIDEO_MEMORY + term)<<12);
}

void from_buf(int term){
    memcpy((VIDEO_MEMORY<<12), (unsigned char*)((VIDEO_MEMORY + term)<<12), 4096);
}

// zero index terminals
void mod_vidmem(int term){
    uint32_t mem = VIDEO_MEMORY + term;
    page_table[VIDEO_MEMORY].P_addr = (char*) mem;
    // curmem = (char*)(mem << 12);

    flush_tlb();
}