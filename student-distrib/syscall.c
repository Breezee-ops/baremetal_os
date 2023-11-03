#include "syscall.h"
#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"
#include "pcb.h"


int32_t halt (uint8_t status) {
    uint32_t currpid;
    currpid = (uint32_t)latest_pid();
    pcb_t* currpcb = get_pcb_ptr(currpid);
    tss.esp0 = 8388608 - (currpcb->parent_pid * 8192) - 4;
    set_exe_page(currpcb->parent_pid);
    free_pid(currpid);
    if(currpcb->parent_pid == 0){
        execute((const uint8_t*) "shell");
    }
    else{
        uint32_t ebp = currpcb->ebp;
        asm volatile ("\n\
        mov %%edx, %%ebp \n\
        mov %%ecx, %%eax \n\
        leave          \n\
        ret            \n\
        "
        ::"d"(ebp), "c"(status)
        );
    }

    return 0; 
}

int context_switch(uint32_t pid){
    uint32_t user_stack_ptr = 8388608 - (pid * 8192);
    tss.esp0 = user_stack_ptr - 4;
    char* user_esp = (char*) 138412028;
    asm volatile (" \n\
    movw %1, %%ds   \n\
    pushl %1        \n\
    pushl %3        \n\
    pushfl          \n\
    pushl %2        \n\
    pushl %0        \n\
    iret            \n\
    "
    ::"r" (eip), "r"(USER_DS), "r" (USER_CS), "r"(user_esp) : "cc"
    );
    return 0;
}

// system calls:
uint8_t exe_check(dentry_t* dirptr){
    uint8_t magic_num[4] = {0x7f, 0x45, 0x4c, 0x46};
    uint8_t buf[4];
    int i;
    file_read(dirptr->inode_num, 0, buf, 4);
    for(i = 0; i < 4; i++){
       if(buf[i] != magic_num[i]) return -1;
    }
    return 0;
}



// TODO: this function takes a command; change this out
int32_t execute(const uint8_t* command){
    dentry_t dir = {
        .filename = "",      // Empty string, initializes all characters to 0 ('\0')
        .filetype = 0,       // 0 for uint32_t
        .inode_num = 0,      // 0 for uint32_t
        .reserved = {0}      // Initializes all elements of the reserved array to 0
    };
    uint8_t filename[FILENAME_LEN] = {0};
    int ind;
    for(ind = 0; ind < FILENAME_LEN && command[ind] != ' ' && command[ind] != '\0'; ind++){
        filename[ind] = command[ind];
    }
    dentry_t* dirptr = &dir; 
    if(read_dentry_by_name((uint8_t*)filename, dirptr) != 0) return -1;
    if(exe_check(dirptr) != 0) return -1;
    // dereference 8mb - 4kb for shell and 16kb for process and put the pcb struct type obj at the top of these two stacks.
    read_data(dirptr->inode_num, 24, (uint8_t*)&eip, 4);
    // cs is user_cs in inline assembly pushfl for eflags ds is set to user_ds
    uint32_t pid = find_pid();
    pcb_t* currpcb = pcb_init(pid);
    set_exe_page(pid);
    // read the executable into memory
    read_data(dirptr->inode_num, 0, (uint8_t*)0x08048000, 0x300000);
    currpcb->parent_pid = (pid == 0) ? 0 : (pid -1);
    uint32_t esp, ebp;
    asm volatile("\n\
    movl %%esp, %0  \n\
    movl %%ebp, %1  \n\
    " : "=r" (esp), "=r" (ebp) : : "cc");
    currpcb->esp = esp;
    currpcb->ebp = ebp;
    // 
    context_switch(pid);
    return 0;
}


int32_t read (uint32_t fd, void* buf, uint32_t nbytes) {
    return term_read(buf, nbytes);
}

int32_t write (uint32_t fd, const void* buf, uint32_t nbytes) {
    term_write(buf, nbytes);
    return 0;
}

int32_t open(const uint8_t* filename) {
    printf("reached open");
    return 0;
}

int32_t close (int32_t fd) {
    printf("reached close");
    return 0;
}

