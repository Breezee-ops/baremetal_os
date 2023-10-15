#include "idt.h"
#include "x86_desc.h"
#include "functions.h"
#include "assembly_linkage.h"

void init_idt(idt_desc_t* idt){
    int i;
    // these are the exceptions
    for(i = 0; i < 20; i++){ 
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].dpl = 0;
        idt[i].present = 1;
    }
    //maskeable interrupts
    // keyboard interrupt
    idt[0x21].seg_selector = KERNEL_CS;
    idt[0x21].reserved4 = 0;
    idt[0x21].reserved3 = 0;
    idt[0x21].reserved2 = 1;
    idt[0x21].reserved1 = 1;
    idt[0x21].reserved0 = 0;
    idt[0x21].size = 1;
    idt[0x21].dpl = 0;
    idt[0x21].present = 1;

    // rtc interrupt
    idt[0x28].seg_selector = KERNEL_CS;
    idt[0x28].reserved4 = 0;
    idt[0x28].reserved3 = 0;
    idt[0x28].reserved2 = 1;
    idt[0x28].reserved1 = 1;
    idt[0x28].reserved0 = 0;
    idt[0x28].size = 1;
    idt[0x28].dpl = 0;
    idt[0x28].present = 1;
    
    // TODO: fix this for the system calls, typically start at 0x80 (correct in doc)
    idt[0x80].reserved3 = 1;
    idt[0x80].present = 1;
    idt[0x80].dpl = 3;

    // set idt index and then the function pointer
    SET_IDT_ENTRY(idt[0x0], diverror_asm);
    SET_IDT_ENTRY(idt[0x1], debug_asm);
    SET_IDT_ENTRY(idt[0x2], nmi_asm);
    SET_IDT_ENTRY(idt[0x3], breakpoint_asm);
    SET_IDT_ENTRY(idt[0x4], overflow_asm);
    SET_IDT_ENTRY(idt[0x5], boundrange_asm);
    SET_IDT_ENTRY(idt[0x6], invalidop_asm);
    SET_IDT_ENTRY(idt[0x7], device_na_asm);
    SET_IDT_ENTRY(idt[0x8], doublefault_asm);
    SET_IDT_ENTRY(idt[0x9], coprocessor_asm);
    SET_IDT_ENTRY(idt[0xa], invalidtss_asm);
    SET_IDT_ENTRY(idt[0xb], segment_na_asm);
    SET_IDT_ENTRY(idt[0xc], stack_seg_asm);
    SET_IDT_ENTRY(idt[0xd], general_protection_asm);
    SET_IDT_ENTRY(idt[0xe], page_fault_asm);
    // f is reserved
    SET_IDT_ENTRY(idt[0x10], floating_excep_asm);
    SET_IDT_ENTRY(idt[0x11], alignment_check_asm);
    SET_IDT_ENTRY(idt[0x12], machine_check_asm);
    SET_IDT_ENTRY(idt[0x13], smd_asm);

}
