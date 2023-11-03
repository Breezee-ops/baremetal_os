#ifndef INTERRUPT_WRAP_H
#define INTERRUPT_WARP_H

#ifndef ASM
extern void rtc_handler_asm();
extern void keyboard_handler_asm();
extern void general_prot_asm();
extern void page_fault_asm();

#endif

#endif

