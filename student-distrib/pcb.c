#include "pcb.h"
#include "term.h"

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
        .read = term_read,
        .write = NULL,
    };

    file_operation_t stdout = {
        .open = NULL,
        .close = NULL,
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

int find_pid() {
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
int free_pid(uint32_t pid) {
    if(active_processes[pid] == 1) {
        active_processes[pid] = 0;
    } else {
        return -1;
    }
        
    return 0;
}

int latest_pid(){
    int i;
    int val;
    for(i = 0; i < 6; i++) {
        if(active_processes[i] == 0) {
            val = (i == 0) ? 0 : i - 1;
            return val;
        }
    }
    return -1;
}
