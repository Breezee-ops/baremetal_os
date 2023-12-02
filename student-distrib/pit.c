#include "pit.h"
#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "term.h"
#include "syscall.h"

#define PIT_DATA_PORT = 0x40
#define PIT_COM_BYTE = 0x36; 
#define PIT_COM_REG = 0x43
#define FREQUENCY = 1193180; 
#define HZ = 10; 

void pit_init()
{
    int divisor = FREQUENCY / HZ;     /* Calculate our divisor */
    outb(PIT_COM_BYTE, PIT_COM_REG);     /* Set our command byte 0x36 */
    outb(divisor & 0xFF, PIT_DATA_PORT);   /* Set low byte of divisor */
    outb(divisor >> 8, PIT_DATA_PORT);     /* Set high byte of divisor */
}

void pit_handler() {

    //begin critical section stop all interrupts
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
    pcbarr[cur_pcb_ptr->pid] = *cur_pcb_ptr;
    curr_term[scheduleIdx].term_pcb = *cur_pcb_ptr;
    
    // change curr terminal cursor
    set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);

    write_to_buf(scheduleIdx); 

    // change curr terminal pcb pointer and idx
    scheduleIdx = (scheduleIdx+1)%3;

    cur_pcb_ptr = get_pcb_ptr(curr_term[scheduleIdx].term_pcb.pid);

    send_eoi(1);
    sti();

    // set new ebp and esp for user execution
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
    return;
}
    

