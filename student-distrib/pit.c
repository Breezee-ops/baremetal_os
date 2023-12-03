#include "pit.h"
#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "term.h"
#include "syscall.h"

#define PIT_DATA_PORT 0x40
#define PIT_COM_BYTE 0x37 
#define PIT_COM_REG 0x43
#define FREQUENCY 300
#define HZ 10


void pit_init()
{
    uint8_t divisor = FREQUENCY / HZ;     /* Calculate our divisor */
    outb(PIT_COM_BYTE, PIT_COM_REG);     /* Set our command byte 0x36 */
    outb((uint8_t)(divisor), PIT_DATA_PORT);   /* Set low byte of divisor */
    outb((uint8_t)(divisor >> 8), PIT_DATA_PORT);     /* Set high byte of divisor */

    scheduleIdx = 0;

    enable_irq(0); 
}

void pit_handler() {
    cli();
    uint32_t esp, ebp;
    asm volatile("\n\
    movl %%esp, %0  \n\
    movl %%ebp, %1  \n\
    " : "=r" (esp), "=r" (ebp) : : "cc");

    // to_buf(scheduleIdx + 1);//delete after vidmem mapping
    // from_buf(((scheduleIdx + 1) % 3) + 1);//delete after vidmem mapping
    curr_term[scheduleIdx].esp = esp;
    curr_term[scheduleIdx].ebp = ebp;
    int next = (scheduleIdx + 1) % 3;
    if(cur_pcb_ptr == NULL){
        send_eoi(0);
        sti();
        return;
    }
    pcbarr[cur_pcb_ptr->pid] = *cur_pcb_ptr;
    curr_term[scheduleIdx].term_pcb = *cur_pcb_ptr;
    set_curr_pos(curr_term[next].x_pos, curr_term[next].y_pos);

    cur_pcb_ptr = get_pcb_ptr(curr_term[next].term_pcb.pid);

    // uint32_t ebp = cur_pcb_ptr->ebp;
    // uint32_t esp = cur_pcb_ptr->esp;
    // pcb_t* old_ptr;
    // // // uint32_t currpid;
    // // // currpid = curr_term[1].term_pcb.pid;
    // // // *cur_pcb_ptr = curr_term[1].term_pcb;

    // // // uint32_t ebp = cur_pcb_ptr->ebp;
    // // // uint32_t esp = cur_pcb_ptr->esp;

    // old_ptr = get_pcb_ptr(curr_term[0].term_pcb.pid);
    // set_exe_page(new_ptr->pid);

    scheduleIdx = (scheduleIdx + 1) % 3;

    // if(termIdx != scheduleIdx) {
    //     page_table[VIDEO_MEMORY].P_addr = (VIDEO_MEMORY + scheduleIdx + 1)<<12;
    //     //remember to change vidmap address for fish to work
    // } else {//if scheduler on current terminal
    //     //dont change paging
    //     page_table[VIDEO_MEMORY].P_addr = (VIDEO_MEMORY)<<12;
    //      //remember to add vidmap address for fish
    // }
    // flush_tlb();
    if(termIdx == scheduleIdx){
        page_vidmap_sched(0);
        write_to_buf(0);
    }
    else{
        page_vidmap_sched(scheduleIdx + 1);
        write_to_buf(scheduleIdx + 1);
    }

    if(curr_term[scheduleIdx].status == 0){
        curr_term[scheduleIdx].status = 1; 
        send_eoi(0);
        sti();
        execute((const uint8_t*) "shell");
        return;
    }
    else {
        ebp = curr_term[scheduleIdx].ebp;
        esp = curr_term[scheduleIdx].esp;
        set_exe_page(cur_pcb_ptr->pid);
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 8388608 - (cur_pcb_ptr->pid * 8192);
        asm volatile ("\n\
        mov %%edx, %%ebp \n\
        mov %%ebx, %%esp \n\
        "
        ::"d"(ebp), "b"(esp) : "memory"
        );
        send_eoi(0);
        sti();
        return;
    }
}