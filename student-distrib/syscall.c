#include "syscall.h"
#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"
#include "pcb.h"

uint8_t active_processes[6] = {0,0,0,0,0,0};
pcb_t* cur_pcb_ptr = NULL;
uint32_t cur_pid = -1;

int32_t halt (uint8_t status) {
    
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
    cur_pcb_ptr = currpcb;
    pcb_init(pid);
    set_exe_page(pid);
    // read the executable into memory
    read_data(dirptr->inode_num, 0, (uint8_t*)0x08048000, 0x300000);
    cur_pcb_ptr->parent_pid = (pid == 0) ? 0 : (pid -1);
    uint32_t esp, ebp;
    asm volatile("\n\
    movl %%esp, %0  \n\
    movl %%ebp, %1  \n\
    " : "=r" (esp), "=r" (ebp) : : "cc");
    cur_pcb_ptr->esp = esp;
    cur_pcb_ptr->ebp = ebp;
    // 
    context_switch(pid);
    return 0;
}


// int32_t read (int32_t fd, void* buf, int32_t nbytes) {
//     printf("reached read");
//     return 0;
// }

// int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
//     printf("reached write");
//     return 0;
// }

// int32_t open(const uint8_t* filename) {
//     printf("reached open");
//     return 0;
// }

// int32_t close (int32_t fd) {
//     printf("reached close");
//     return 0;
// }

int32_t read (int32_t fd, void* buf, int32_t nbytes){

    if(fd < 0 || nbytes < 0){
        return -1;
    }
    if(cur_pcb_ptr->fda[0].flags == 1){
        return -1;
    }
    int32_t ret = cur_pcb_ptr->fda[0].file_operations->read(fd, buf, nbytes);
    return ret;
}

pcb_t* pcb_init(uint32_t pid) {
    pcb_t cur_pcb;

    //initialize empty fd array
    memset(cur_pcb.fda, 0, sizeof(fd_t) * 8);

    file_operation_t stdin = {
        .open = NULL,
        .close = term_close,
        .read = term_read,
        .write = NULL,
    };

    file_operation_t stdout = {
        .open = NULL,
        .close = term_close,
        .read = NULL,
        .write = term_write,
    };

    cur_pcb.fda[0].file_operations = &stdin;
    cur_pcb.fda[0].inode = NULL;
    cur_pcb.fda[0].file_position = 0;
    cur_pcb.fda[0].flags = 1;//show filled by stdin

    cur_pcb.fda[1].file_operations = &stdout;
    cur_pcb.fda[1].inode = NULL;
    cur_pcb.fda[1].file_position = 0;
    cur_pcb.fda[1].flags = 1;//show filled by stdout

    cur_pcb.esp = 0;
    cur_pcb.ebp = 0;
    cur_pcb.pid = pid;
    cur_pcb.parent_pid = -1;

    pcb_t* cur_pcb_ptr = &cur_pcb;
    return cur_pcb_ptr;
}

pcb_t* get_pcb_ptr(uint32_t pid) {
    return (pcb_t *)(0x800000 - (pid + 1) * 0x2000);
}

uint32_t find_pid() {
    int i;
    uint32_t found;

    for(i = 0; i < 6; i++) {
        if(active_processes[i] == 0) {
            active_processes[i] = 1;
            found = i;
            return found;
        }
    }
    return -1;
}

int find_fda_idx() {
    int i;
    int found;

    for(i = 0; i < 8; i++) {
        if(cur_pcb_ptr->fda[i].flags == 0) {//found free spot
            cur_pcb_ptr->fda[i].flags = 1;
            found = i;
            return found;
        }
    }

    return -1;
}

//free the corresponding pid in active_processes
//-1 for fail 0 for success
uint32_t free_pid(uint32_t pid) {
    if(active_processes[pid] == 1) {
        active_processes[pid] = 0;
    } else {
        return -1;
    }
        
    return 0;
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){

    if(fd < 0 || nbytes < 0){
        return -1;
    }
    if(cur_pcb_ptr->fda[fd].flags == 1){
        return -1;
    }
    int32_t ret = cur_pcb_ptr->fda[fd].file_operations->write(fd, buf, nbytes);
    return ret;
}

int32_t open (const uint8_t* filename){
    int fda_idx;
    if(strlen((char*)filename) == NULL){
        return -1;
    }

    fda_idx = find_fda_idx();

    file_operation_t stdin = {
        .open = NULL,
        .close = term_close,
        .read = term_read,
        .write = NULL,
    };

    file_operation_t stdout = {
        .open = NULL,
        .close = term_close,
        .read = NULL,
        .write = term_write,
    };

    if(fda_idx == -1)//couldnt find a free spot in fda
        return -1;
    
    if(strncmp("stdin", (char*)filename, 6) == 0) {
        cur_pcb_ptr->fda[fda_idx].file_operations = &stdin;
    } else if(strncmp("stdout", (char*)filename, 7) == 0) {
        cur_pcb_ptr->fda[fda_idx].file_operations = &stdout;
    }
    // dentry_t data_entry;

    // for(i = 0; i < 8; i++) {
    //     if(cur_pcb_ptr->fda[i].flags == 1){
    //         cur_pcb_ptr->fda[i].flags = 0;
    //         cur_pcb_ptr->fda[i].inode = data_entry.inode_num;
    //         cur_pcb_ptr->fda[i].file_position = i;
    //         break;
    //     }
    //     return i; 
    // }

    return fda_idx;
}

int32_t close (int32_t fd){

    if(cur_pcb_ptr->fda[fd].flags == 0){
        return -1;
    }

    cur_pcb_ptr->fda[fd].flags = 0;
    cur_pcb_ptr->fda[fd].file_position = 0;
    cur_pcb_ptr->fda[fd].inode = NULL;
    free_pid(cur_pid);

    return 0;
}

