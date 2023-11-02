#include "pcb.h"


uint8_t active_processes[6] = {0,0,0,0,0,0};

pcb_t* pcb_init(uint32_t pid) {
    pcb_t cur_pcb;

    //initialize empty fd array
    memset(cur_pcb.fda, 0, sizeof(fd_t) * 8);

    cur_pcb.esp = 0;
    cur_pcb.ebp = 0;
    cur_pcb.pid = pid;
    cur_pcb.parent_pid = -1;

    pcb_t* pcb_ptr = &cur_pcb;
    return pcb_ptr;
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

