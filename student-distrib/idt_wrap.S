#define ASM 1
#include "idt_wrap.h"

.globl syscall_handler_asm

.data 
    NUM_SYSCALLS = 6

//handles assembly linkage to the sycall_handler function in c

// syscall number in eax
// first arg ebx
// second arg ecx
// third arg edx

syscall_handler_asm:

    //save flags
    pushal
    pushfl

    // value checking 
    cmpl $0, %eax
    jl bad_command
    cmpl NUM_SYSCALLS, %eax
    jg bad_command

    // jump table here
    jmp *jump_table(,%eax, 4)
    
    bad_command: 
        mov $-1, %eax
        iret

    //restore flags
    popfl
    popal
    iret

jump_table: 
	.long syscall_handler






