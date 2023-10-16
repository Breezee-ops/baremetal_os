#define ASM 1

#include "rtc.h"
#include "functions.h"

.globl rtc_handler_asm

rtc_handler_asm:
    pushal
    pushfl
    call init_rtc
    popfl
    popal
    iret



.globl keyboard_handler_asm
keyboard_handler_asm: 
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret