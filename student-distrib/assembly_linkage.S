#include "idt.h"
#include "functions.h"

; .globl call_intr
; call_intr:
		
; 		mov 8(%esp), %eax # eax now stores the cmd
; 		cmpl $0, %eax
; 		jl bad_command
; 		cmpl $4, %eax
; 		jg bad_command
; 		jmp *jump_table(,%eax, 4)

; 	bad_command: 

; 		mov $-1, %eax
; 		ret

; jump_table: 
; 	.long mp1_ioctl_startgame, mp1_ioctl_addmissile, mp1_ioctl_movexhairs, mp1_ioctl_getstatus, mp1_ioctl_endgame

.globl DIV_ZERO_asm, DEBUG_EX_asm, NMI_INT_asm

DIV_ZERO_asm:
    pushal
    pushfl
    call DIVZERO
    popfl
    popal
    iret

DEBUG_EX_asm:
    pushal
    pushfl
    call DEBUG_EX
    popfl
    popal
    iret

NMI_INT_asm:
    pushal
    pushfl
    call NMI_INT
    popfl
    popal
    iret