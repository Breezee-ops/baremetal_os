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

.globl diverror_asm debug_asm nmi_asm breakpoint_asm overflow_asm boundrange_asm invalidop_asm device_na_asm doublefault_asm
.globl invalidtss_asm segment_na_asm stack_seg_asm general_protection_asm page_fault_asm floating_excep_asm alignment_check_asm 
.globl machine_check_asm smd_asm coprocessor_asm

diverror_asm: 
pushal
pushfl
call diverror
popal
popflcall
iret

debug_asm:
pushal
pushfl
call debug
popal
popfl
iret

nmi_asm:
pushal
pushfl
call nmi
popal
popfl
iret

breakpoint_asm:
pushal
pushfl
call breakpoint
popal
popfl
iret

overflow_asm:
pushal
pushfl
call overflow
popal
popfl
iret

boundrange_asm:
pushal
pushfl
call boundrange
popal
popfl
iret

invalidop_asm:
pushal
pushfl
call invalidop
popal
popfl
iret

device_na_asm:
pushal
pushfl
call device_na
popal
popfl
iret

doublefault_asm:
pushal
pushfl
call doublefault
popal
popfl
iret

invalidtss_asm:
pushal
pushfl
call invalidtss
popal
popfl
iret

segment_na_asm:
pushal
pushfl
call segment_na
popal
popfl
iret

stack_seg_asm:
pushal
pushfl
call stack_seg
popal
popfl
iret

general_protection_asm:
pushal
pushfl
call general_protection
popal
popfl
iret

page_fault_asm:
pushal
pushfl
call page_fault
popal
popfl
iret

floating_excep_asm:
pushal
pushfl
call floating_excep
popal
popfl
iret

alignment_check_asm:
pushal
pushfl
call alignment_check
popal
popfl
iret

machine_check_asm:
pushal
pushfl
call machine_check
popal
popfl
iret

smd_asm:
pushal
pushfl
call smd
popal
popfl
iret

coprocessor_asm:
pushal
pushfl
call coprocessor
popal
popfl
iret