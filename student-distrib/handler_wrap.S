#define ASM 1
#include "handler_wrap.h"

.globl rtc_handler_asm

//handles assembly linkage to the rtc_handler function in c
rtc_handler_asm:
//save flags
    pushal
    pushfl
    call rtc_handler
//restore flags
    popfl
    popal
    iret

.globl keyboard_handler_asm

//handles assembly linkage to the keyboard_handler_asm
keyboard_handler_asm: 
//save flags
    pushal
    pushfl
    call keyboard_handler
//restore flags
    popfl
    popal
    iret

    



    