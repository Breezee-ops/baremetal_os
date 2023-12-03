#include "pit.h"
#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "term.h"
#include "syscall.h"

#define PIT_DATA_PORT 0x40
#define PIT_COM_BYTE 0x37 
#define PIT_COM_REG 0x43
#define FREQUENCY 1193200
#define HZ 100

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
    // save current user program stack
    curr_term[scheduleIdx].esp = esp;
    curr_term[scheduleIdx].ebp = ebp;

    // store the current pcb info 
    if(cur_pcb_ptr == NULL) {
        send_eoi(0);
        sti();
        return;
    }
    pcbarr[cur_pcb_ptr->pid] = *cur_pcb_ptr;
    curr_term[scheduleIdx].term_pcb = *cur_pcb_ptr;
    
    // change curr terminal cursor
    scheduleIdx = (scheduleIdx + 1) % 3;
    set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);
    // change curr terminal pcb pointer and idx
    cur_pcb_ptr = get_pcb_ptr(curr_term[scheduleIdx].term_pcb.pid);
    send_eoi(0);
    sti();

    if(curr_term[scheduleIdx].status == 0){
        curr_term[scheduleIdx].status = 1; 
        execute((const uint8_t*) "shell");
        return;
    }
    else {
        // set new ebp and esp for user execution
        ebp = curr_term[1].ebp;
        esp = curr_term[1].esp;
        set_exe_page(cur_pcb_ptr->pid);
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 8388608 - (cur_pcb_ptr->pid * 8192);
        asm volatile ("\n\
        mov %%edx, %%ebp \n\
        mov %%ebx, %%esp \n\
        "
        ::"d"(ebp), "b"(esp) : "memory"
        );
        return;
    }
    
    // cli();

    // if (curr_term[1].status == 0) {
    //     send_eoi(0);
    //     sti();  
    //     return; 
    // }

    // //begin critical section stop all interrupts
    // uint32_t esp, ebp;
    // asm volatile("\n\
    // movl %%esp, %0  \n\
    // movl %%ebp, %1  \n\
    // " : "=r" (esp), "=r" (ebp) : : "cc");

    // // save current user program stack
    // curr_term[scheduleIdx].esp = esp;
    // curr_term[scheduleIdx].ebp = ebp;
    
    // // store the current pcb info 
    // pcbarr[cur_pcb_ptr->pid] = *cur_pcb_ptr;
    // curr_term[scheduleIdx].term_pcb = *cur_pcb_ptr;
    
    // // change curr terminal cursor
    // set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);

    // write_to_buf(scheduleIdx); 

    // // change curr terminal pcb pointer and idx
    // scheduleIdx = (scheduleIdx+1)%3;

    // cur_pcb_ptr = get_pcb_ptr(curr_term[scheduleIdx].term_pcb.pid);

    // send_eoi(1);
    // sti();

    // // set new ebp and esp for user execution
    // ebp = curr_term[scheduleIdx].ebp;
    // esp = curr_term[scheduleIdx].esp;
    // set_exe_page(cur_pcb_ptr->pid);
    // tss.ss0 = KERNEL_DS;
    // tss.esp0 = 8388608 - (cur_pcb_ptr->pid * 8192);
    // asm volatile ("\n\
    // mov %%edx, %%ebp \n\
    // mov %%ebx, %%esp \n\
    // "
    // ::"d"(ebp), "b"(esp) : "memory"
    // );
    // return;
}
    

