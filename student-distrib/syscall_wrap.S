#define ASM 1
#include "x86_desc.h"

.globl syscall_handler_asm
.align 4
syscall_handler_asm:

    // push all registers
    // pushl %eax
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %ebp
    pushl %esi
    pushl %edi

    // push flags
    // pushal
    pushfl

    // push arguments
    pushl %edx
    pushl %ecx
    pushl %ebx

    // value checking 
    cmpl $0, %eax
    jl bad_command
    cmpl $6, %eax
    jg bad_command

    // jump table here
    call *jump_table(,%eax, 4)
    jmp clean_up
    
    bad_command: 
        mov $-1, %eax

clean_up: 
    // pop arguments
    addl $12, %esp

    // pop flags
    popfl
    // popal

    // pop registers
    popl %edi
    popl %esi
    popl %ebp
    popl %ebx
    popl %edx
    popl %ecx
    // popl %eax
    iret


.align 4
jump_table: 
    .long 0x0
	.long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
