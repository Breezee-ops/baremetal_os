#include "syscall.h"
#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"
#include "rtc.h"

file_operation_t file = {&file_read, &file_write, &file_open, &file_close};
file_operation_t rtc = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};
file_operation_t term = {&term_read, &term_write, &term_open, &term_close};
file_operation_t dir = {&directory_read, &directory_write, &directory_open, &directory_close};

uint8_t active_processes[6] = {0,0,0,0,0,0};
pcb_t pcbarr[6];
pcb_t* cur_pcb_ptr = NULL;
uint32_t cur_pid = 0;

/*  latest_pid
 * Description: returns current pid
 * Inputs: none
 * Outputs: cur_pid
 */
int latest_pid(){
    return cur_pid;
}

/* halt
 * Description: returns to parent process
 * Inputs: return value of the current process
 * Outputs: switches to parent process or start a new shell if no parent
 */
int32_t halt (uint8_t status) {
    uint32_t currpid;
    currpid = cur_pcb_ptr->pid;
    uint32_t ebp = cur_pcb_ptr->ebp;
    uint32_t esp = cur_pcb_ptr->esp;

    cur_pcb_ptr = get_pcb_ptr(cur_pcb_ptr->parent_pid);
    set_exe_page(cur_pcb_ptr->pid);
    free_pid(cur_pcb_ptr->pid);
    if(currpid == 0){
        free_pid(cur_pcb_ptr->pid);
        execute((const uint8_t*) "shell");
    }
    else{
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 8388608 - (cur_pcb_ptr->pid * 8192);
        asm volatile ("\n\
        mov %%edx, %%ebp \n\
        mov %%ecx, %%eax \n\
        mov %%ebx, %%esp \n\
        leave          \n\
        ret            \n\
        "
        ::"d"(ebp), "c"(status),"b"(esp)
        );
    }

    return 0; 
}

/* context_switch
 * Description: sets up new stack for a new process
 * Inputs: pid of the new process
 * Outputs: switches stacks
 */
int context_switch(uint32_t pid){
    uint32_t user_stack_ptr = 8388608 - (pid * 8192);
    tss.esp0 = user_stack_ptr ;
    char* user_esp = (char*) (138412028 );
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

/* exe_check
 * Description: sets up new stack for a new process
 * Inputs: pid of the new process
 * Outputs: switches stacks
 */
uint8_t exe_check(dentry_t* dirptr){
    uint8_t magic_num[4] = {0x7f, 0x45, 0x4c, 0x46};//corresponding process numbers
    uint8_t buf[4];
    int i;
    file_read(dirptr->inode_num, 0, buf, 4);
    for(i = 0; i < 4; i++){
       if(buf[i] != magic_num[i]) return -1;
    }
    return 0;
}



/* execute
 * Description: creates a new process and switches from current process
 * Inputs: command type
 * Outputs: switches to new process
 */
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
    // dereference 8mb kb for shell and 16kb for process and put the pcb struct type obj at the top of these two stacks.
    read_data(dirptr->inode_num, 24, (uint8_t*)&eip, 4);
    // cs is user_cs in inline assembly pushfl for eflags ds is set to user_ds
    uint32_t pid = find_pid();
    cur_pid = pid;
    pcb_t* currpcb = pcb_init(pid);
    cur_pcb_ptr = currpcb;

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
    int it;
    for(it = 0; it < 32; it++){
        if(command[ind] != ' '){
            cur_pcb_ptr->args[it] = command[ind];
        }
        ind++;
    }
    // 
    context_switch(pid);
    return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes){
    if(buf == NULL) return -1;
    int it;
    int bc = 0;
    int ccount;
    int bufc = 0;
    for(it = 0; it < 128; it++){
        if(cur_pcb_ptr->args[it] != ' ' || cur_pcb_ptr->args[it] != '\0' || cur_pcb_ptr->args[it] != '\n') ccount++;
    }
    if(ccount == 0) return -1;
    it = (nbytes > 128) ? 128 : nbytes;
    while(cur_pcb_ptr->args[bc] == '\0' || cur_pcb_ptr->args[bc] == '\n' || cur_pcb_ptr->args[bc] == ' ') bc++;
    while(cur_pcb_ptr->args[bc] != '\0' && cur_pcb_ptr->args[bc] != '\n'){
        buf[bufc] = cur_pcb_ptr->args[bc];
        bufc++;
        bc++;
    }
    return 0;
}

/* read
 * Description: call corresponding read function
 * Inputs: fd: file descriptor, buf: buffer to place read message into, nbytes: length of buffer
 * Outputs: bytes read
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    int bytes;
    if(fd > 1 && cur_pcb_ptr->fda[fd].flags == 0) return -1;
    switch(fd){
        case 0: return term_read(0, 0, buf, nbytes);
        case 1: return -1;
        default: 
            bytes = cur_pcb_ptr->fda[fd].file_operations->read(cur_pcb_ptr->fda[fd].inode_num, cur_pcb_ptr->fda[fd].file_position, buf, nbytes);
            cur_pcb_ptr->fda[fd].file_position += bytes;
    }
    return bytes;
}

/* write
 * Description: call corresponding write function
 * Inputs: fd: file descriptor, buf: write from buffer to terminal , nbytes: length of buffer
 * Outputs: bytes written
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    int bytes;
    if(fd > 1 && cur_pcb_ptr->fda[fd].flags == 0) return -1;
    switch(fd){
        case 0: return -1;
        case 1: return term_write(0, buf, nbytes);
        default: 
            bytes = cur_pcb_ptr->fda[fd].file_operations->write(fd, buf, nbytes);
            cur_pcb_ptr->fda[fd].file_position += bytes;
    }
    return bytes;
}

/* open
 * Description: specifies file type for file operations
 * Inputs: filename
 * Outputs: file descriptor
 */
int32_t open(const uint8_t* filename) {
    dentry_t pos;
    if(read_dentry_by_name(filename, &pos)) return -1;
    int i;
    for(i = 2; i < 8; i++){
        if(cur_pcb_ptr->fda[i].flags == 0){
            cur_pcb_ptr->fda[i].flags = 1;
            break;
        }
    }
    if(i == 8) return -1;
    cur_pcb_ptr->fda[i].file_position = 0;
    cur_pcb_ptr->fda[i].inode_num = pos.inode_num;
    switch(pos.filetype){
        case 0: cur_pcb_ptr->fda[i].file_operations = &rtc;
                break;
        case 1: cur_pcb_ptr->fda[i].file_operations = &dir;
                break;
        case 2: cur_pcb_ptr->fda[i].file_operations = &file;
                break;
        default:
            return -1;
    }
    cur_pcb_ptr->fda[i].file_operations->open(filename);
    return i;
}

/* close
 * Description: closes file and updates fd array
 * Inputs: file descriptor
 * Outputs: success or fail
 */
int32_t close (int32_t fd) {
    if(cur_pcb_ptr->fda[fd].flags == 0) return -1;
    cur_pcb_ptr->fda[fd].flags = 0;
    return cur_pcb_ptr->fda[fd].file_operations->close(fd);
}


/* get_pcb_ptr
 * Description: retrieve ptr to pcb given pid
 * Inputs: pid
 * Outputs: ptr to pcb
 */
pcb_t* get_pcb_ptr(uint32_t pid) {
    return (pcb_t *)(0x800000 - (pid + 1) * 0x2000);
}

/* find_pid
 * Description: finds an open pid
 * Inputs: none
 * Outputs: success or fail
 */
uint32_t find_pid() {
    int i;
    uint32_t found;

    for(i = 0; i < 6; i++) {
        if(active_processes[i] == 0) {
            active_processes[i] = 1;
            found = i;

            cur_pid = i;
            return found;
        }
    }
    return -1;
}

/* find_fda_idx
 * Description: finds an open fd slot in fda
 * Inputs: none
 * Outputs: success or fail
 */
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

/* free_pid
 * Description: free the corresponding pid in active_processes
 * Inputs: pid
 * Outputs: success or fail
 */
uint32_t free_pid(uint32_t pid) {
    if(active_processes[pid] == 1) {
        active_processes[pid] = 0;
    } else {
        return -1;
    }
        
    return 0;
}

/* pcb_init
 * Description: initalizes a pcb
 * Inputs: pid
 * Outputs: success or fail
 */
pcb_t* pcb_init(uint32_t pid){
    active_processes[pid] = 1;
    pcbarr[pid].parent_pid = (pid == 0) ? 0 : pid - 1;
    pcbarr[pid].ebp = 0;
    pcbarr[pid].esp = 0;
    pcbarr[pid].pid = pid;
    return &pcbarr[pid];
}

// int32_t read (int32_t fd, void* buf, int32_t nbytes){

//     if(fd < 0 || nbytes < 0){
//         return -1;
//     }
//     if(cur_pcb_ptr->fda[0].flags == 1){
//         return -1;
//     }
//     int32_t ret = cur_pcb_ptr->fda[0].file_operations->read(fd, buf, nbytes);
//     return ret;
// }

// int32_t write (int32_t fd, const void* buf, int32_t nbytes){

//     if(fd < 0 || nbytes < 0){
//         return -1;
//     }
//     if(cur_pcb_ptr->fda[fd].flags == 1){
//         return -1;
//     }
//     int32_t ret = cur_pcb_ptr->fda[fd].file_operations->write(fd, buf, nbytes);
//     return ret;
// }

// int32_t open (const uint8_t* filename){
//     int fda_idx;
//     if(strlen((char*)filename) == NULL){
//         return -1;
//     }

//     fda_idx = find_fda_idx();

//     file_operation_t stdin = {
//         .open = NULL,
//         .close = term_close,
//         .read = term_read,
//         .write = NULL,
//     };

//     file_operation_t stdout = {
//         .open = NULL,
//         .close = term_close,
//         .read = NULL,
//         .write = term_write,
//     };

//     if(fda_idx == -1)//couldnt find a free spot in fda
//         return -1;
    
//     if(strncmp("stdin", (char*)filename, 6) == 0) {
//         cur_pcb_ptr->fda[fda_idx].file_operations = &stdin;
//     } else if(strncmp("stdout", (char*)filename, 7) == 0) {
//         cur_pcb_ptr->fda[fda_idx].file_operations = &stdout;
//     }
//     // dentry_t data_entry;

//     // for(i = 0; i < 8; i++) {
//     //     if(cur_pcb_ptr->fda[i].flags == 1){
//     //         cur_pcb_ptr->fda[i].flags = 0;
//     //         cur_pcb_ptr->fda[i].inode = data_entry.inode_num;
//     //         cur_pcb_ptr->fda[i].file_position = i;
//     //         break;
//     //     }
//     //     return i; 
//     // }

//     return fda_idx;
// }

// int32_t close (int32_t fd){

//     if(cur_pcb_ptr->fda[fd].flags == 0){
//         return -1;
//     }

//     cur_pcb_ptr->fda[fd].flags = 0;
//     cur_pcb_ptr->fda[fd].file_position = 0;
//     cur_pcb_ptr->fda[fd].inode = NULL;
//     free_pid(cur_pid);

//     return 0;
// }

#include "syscall.h"
#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"
#include "rtc.h"

file_operation_t file = {&file_read, &file_write, &file_open, &file_close};
file_operation_t rtc = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};
file_operation_t term = {&term_read, &term_write, &term_open, &term_close};
file_operation_t dir = {&directory_read, &directory_write, &directory_open, &directory_close};

uint8_t active_processes[6] = {0,0,0,0,0,0};
pcb_t pcbarr[6];
pcb_t* cur_pcb_ptr = NULL;
uint32_t cur_pid = 0;

/*  latest_pid
 * Description: returns current pid
 * Inputs: none
 * Outputs: cur_pid
 */
int latest_pid(){
    return cur_pid;
}

/* halt
 * Description: returns to parent process
 * Inputs: return value of the current process
 * Outputs: switches to parent process or start a new shell if no parent
 */
int32_t halt (uint8_t status) {
    uint32_t currpid;
    currpid = cur_pcb_ptr->pid;
    uint32_t ebp = cur_pcb_ptr->ebp;
    uint32_t esp = cur_pcb_ptr->esp;

    cur_pcb_ptr = get_pcb_ptr(cur_pcb_ptr->parent_pid);
    set_exe_page(cur_pcb_ptr->pid);
    free_pid(cur_pcb_ptr->pid);
    if(cur_pcb_ptr->pid == 0){
        free_pid(cur_pcb_ptr->pid);
        execute((const uint8_t*) "shell");
    }
    else{
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 8388608 - (cur_pcb_ptr->pid * 8192);
        asm volatile ("\n\
        mov %%edx, %%ebp \n\
        mov %%ecx, %%eax \n\
        mov %%ebx, %%esp \n\
        leave          \n\
        ret            \n\
        "
        ::"d"(ebp), "c"(status),"b"(esp)
        );
    }

    return 0; 
}

/* context_switch
 * Description: sets up new stack for a new process
 * Inputs: pid of the new process
 * Outputs: switches stacks
 */
int context_switch(uint32_t pid){
    uint32_t user_stack_ptr = 8388608 - (pid * 8192);
    tss.esp0 = user_stack_ptr ;
    char* user_esp = (char*) (138412028 );
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

/* exe_check
 * Description: sets up new stack for a new process
 * Inputs: pid of the new process
 * Outputs: switches stacks
 */
uint8_t exe_check(dentry_t* dirptr){
    uint8_t magic_num[4] = {0x7f, 0x45, 0x4c, 0x46};//corresponding process numbers
    uint8_t buf[4];
    int i;
    file_read(dirptr->inode_num, 0, buf, 4);
    for(i = 0; i < 4; i++){
       if(buf[i] != magic_num[i]) return -1;
    }
    return 0;
}



/* execute
 * Description: creates a new process and switches from current process
 * Inputs: command type
 * Outputs: switches to new process
 */
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
    // dereference 8mb kb for shell and 16kb for process and put the pcb struct type obj at the top of these two stacks.
    read_data(dirptr->inode_num, 24, (uint8_t*)&eip, 4);
    // cs is user_cs in inline assembly pushfl for eflags ds is set to user_ds
    uint32_t pid = find_pid();
    cur_pid = pid;
    pcb_t* currpcb = pcb_init(pid);
    cur_pcb_ptr = currpcb;

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

/* read
 * Description: call corresponding read function
 * Inputs: fd: file descriptor, buf: buffer to place read message into, nbytes: length of buffer
 * Outputs: bytes read
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    int bytes;
    if(fd > 1 && cur_pcb_ptr->fda[fd].flags == 0) return -1;
    switch(fd){
        case 0: return term_read(0, 0, buf, nbytes);
        case 1: return -1;
        default: 
            bytes = cur_pcb_ptr->fda[fd].file_operations->read(cur_pcb_ptr->fda[fd].inode_num, cur_pcb_ptr->fda[fd].file_position, buf, nbytes);
            if(cur_pcb_ptr->fda[fd].){
                cur_pcb_ptr->fda[fd].file_position += 1;
            }
            else{
                cur_pcb_ptr->fda[fd].file_position += bytes;
            }
            cur_pcb_ptr->fda[fd].inode_num++;
    }
    return bytes;
}

/* write
 * Description: call corresponding write function
 * Inputs: fd: file descriptor, buf: write from buffer to terminal , nbytes: length of buffer
 * Outputs: bytes written
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    int bytes;
    if(fd > 1 && cur_pcb_ptr->fda[fd].flags == 0) return -1;
    switch(fd){
        case 0: return -1;
        case 1: return term_write(0, buf, nbytes);
        default: 
            bytes = cur_pcb_ptr->fda[fd].file_operations->write(fd, buf, nbytes);
            cur_pcb_ptr->fda[fd].file_position += bytes;
    }
    return bytes;
}

/* open
 * Description: specifies file type for file operations
 * Inputs: filename
 * Outputs: file descriptor
 */
int32_t open(const uint8_t* filename) {
    dentry_t pos;
    if(read_dentry_by_name(filename, &pos)) return -1;
    int i;
    for(i = 2; i < 8; i++){
        if(cur_pcb_ptr->fda[i].flags == 0){
            cur_pcb_ptr->fda[i].flags = 1;
            break;
        }
    }
    if(i == 8) return -1;
    cur_pcb_ptr->fda[i].file_position = 0;
    cur_pcb_ptr->fda[i].inode_num = pos.inode_num;
    switch(pos.filetype){
        case 0: cur_pcb_ptr->fda[i].file_operations = &rtc;
                break;
        case 1: cur_pcb_ptr->fda[i].file_operations = &dir;
                break;
        case 2: cur_pcb_ptr->fda[i].file_operations = &file;
                break;
        default:
            return -1;
    }
    cur_pcb_ptr->fda[i].file_operations->open(filename);
    return i;
}

/* close
 * Description: closes file and updates fd array
 * Inputs: file descriptor
 * Outputs: success or fail
 */
int32_t close (int32_t fd) {
    if(cur_pcb_ptr->fda[fd].flags == 0) return -1;
    cur_pcb_ptr->fda[fd].flags = 0;
    return cur_pcb_ptr->fda[fd].file_operations->close(fd);
}

int32_t vidmap (uint8_t** screen_start){
    return 0;
}


/* get_pcb_ptr
 * Description: retrieve ptr to pcb given pid
 * Inputs: pid
 * Outputs: ptr to pcb
 */
pcb_t* get_pcb_ptr(uint32_t pid) {
    return (pcb_t *)(0x800000 - (pid + 1) * 0x2000);
}

/* find_pid
 * Description: finds an open pid
 * Inputs: none
 * Outputs: success or fail
 */
uint32_t find_pid() {
    int i;
    uint32_t found;

    for(i = 0; i < 6; i++) {
        if(active_processes[i] == 0) {
            active_processes[i] = 1;
            found = i;

            cur_pid = i;
            return found;
        }
    }
    return -1;
}

/* find_fda_idx
 * Description: finds an open fd slot in fda
 * Inputs: none
 * Outputs: success or fail
 */
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

/* free_pid
 * Description: free the corresponding pid in active_processes
 * Inputs: pid
 * Outputs: success or fail
 */
uint32_t free_pid(uint32_t pid) {
    if(active_processes[pid] == 1) {
        active_processes[pid] = 0;
    } else {
        return -1;
    }
        
    return 0;
}

/* pcb_init
 * Description: initalizes a pcb
 * Inputs: pid
 * Outputs: success or fail
 */
pcb_t* pcb_init(uint32_t pid){
    active_processes[pid] = 1;
    pcbarr[pid].parent_pid = (pid == 0) ? 0 : pid - 1;
    pcbarr[pid].ebp = 0;
    pcbarr[pid].esp = 0;
    pcbarr[pid].pid = pid;
    return &pcbarr[pid];
}

// int32_t read (int32_t fd, void* buf, int32_t nbytes){

//     if(fd < 0 || nbytes < 0){
//         return -1;
//     }
//     if(cur_pcb_ptr->fda[0].flags == 1){
//         return -1;
//     }
//     int32_t ret = cur_pcb_ptr->fda[0].file_operations->read(fd, buf, nbytes);
//     return ret;
// }

// int32_t write (int32_t fd, const void* buf, int32_t nbytes){

//     if(fd < 0 || nbytes < 0){
//         return -1;
//     }
//     if(cur_pcb_ptr->fda[fd].flags == 1){
//         return -1;
//     }
//     int32_t ret = cur_pcb_ptr->fda[fd].file_operations->write(fd, buf, nbytes);
//     return ret;
// }

// int32_t open (const uint8_t* filename){
//     int fda_idx;
//     if(strlen((char*)filename) == NULL){
//         return -1;
//     }

//     fda_idx = find_fda_idx();

//     file_operation_t stdin = {
//         .open = NULL,
//         .close = term_close,
//         .read = term_read,
//         .write = NULL,
//     };

//     file_operation_t stdout = {
//         .open = NULL,
//         .close = term_close,
//         .read = NULL,
//         .write = term_write,
//     };

//     if(fda_idx == -1)//couldnt find a free spot in fda
//         return -1;
    
//     if(strncmp("stdin", (char*)filename, 6) == 0) {
//         cur_pcb_ptr->fda[fda_idx].file_operations = &stdin;
//     } else if(strncmp("stdout", (char*)filename, 7) == 0) {
//         cur_pcb_ptr->fda[fda_idx].file_operations = &stdout;
//     }
//     // dentry_t data_entry;

//     // for(i = 0; i < 8; i++) {
//     //     if(cur_pcb_ptr->fda[i].flags == 1){
//     //         cur_pcb_ptr->fda[i].flags = 0;
//     //         cur_pcb_ptr->fda[i].inode = data_entry.inode_num;
//     //         cur_pcb_ptr->fda[i].file_position = i;
//     //         break;
//     //     }
//     //     return i; 
//     // }

//     return fda_idx;
// }

// int32_t close (int32_t fd){

//     if(cur_pcb_ptr->fda[fd].flags == 0){
//         return -1;
//     }

//     cur_pcb_ptr->fda[fd].flags = 0;
//     cur_pcb_ptr->fda[fd].file_position = 0;
//     cur_pcb_ptr->fda[fd].inode = NULL;
//     free_pid(cur_pid);

//     return 0;
// }

