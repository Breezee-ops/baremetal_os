#include "pcb.h"

uint8_t active_processes[6] = {0,0,0,0,0,0};
pcb_t* cur_pcb_ptr = NULL;
uint32_t cur_pid = -1;

pcb_t* pcb_init(uint32_t pid) {
    pcb_t cur_pcb;

    //initialize empty fd array
    memset(cur_pcb.fda, 0, sizeof(fd_t) * 8);

    file_operation_t stdin = {
        .open = NULL,
        .close = NULL,
        .read = terminal_read,
        .write = NULL,
    };

    file_operation_t stdout = {
        .open = NULL,
        .close = NULL,
        .read = NULL,
        .write =terminal_write,
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

//to be filled by zach and TJ's code
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    return 0; 
}
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    return 0;
}


// int32_t halt (uint8_t status) {
//     int i;

//     // free_pid(cur_pid);

//     //if no parent shell then call shell
//     if(cur_pcb_ptr->parent_pid == -1) {
//         execute((uint8_t *)"shell");
//     }

//     tss.esp0 = (0x800000 - (cur_pcb_ptr->parent_pid - 1) * 0x2000) - 4;

//     //free all fds in the fda
//     for(int i = 2; i < 6; i++) {
//         cur_pcb_ptr->fda[i].flags = 0;
//     }

//     cur_pid = cur_pcb_ptr = cur_pcb_ptr->parent_pid;
//     cur_pcb_ptr = get_pcb_ptr(cur_pcb_ptr->parent_pid);
    
// }
