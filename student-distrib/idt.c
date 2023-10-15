#include "idt.h"
#include "x86_desc.h"

void init_idt(idt_desc_t* idt){
    int i;
    for(i = 0; i < NUM_VEC; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].dpl = 0;
        idt[i].present = 0;
        if(i == 33 || i == 34 || i == 40){ // these are for interrupts, interrupts are from 32 - 256 in IDT
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4 = 0;
            idt[i].reserved3 = 0;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            idt[i].reserved0 = 0;
            idt[i].size = 1;
            idt[i].dpl = 0;
            idt[i].present = 1;
        }
    }
    // TODO: fix this for the system calls, typically start at 0x80 (correct in doc)
    idt[0x80].seg_selector = KERNEL_CS;
    idt[0x80].reserved4 = 0;
    idt[0x80].reserved3 = 0;
    idt[0x80].reserved2 = 1;
    idt[0x80].reserved1 = 1;
    idt[0x80].reserved0 = 0;
    idt[0x80].size = 1;
    idt[0x80].dpl = 0;
    idt[0x80].present = 1;

    for(i = 0; i < 32; i++){ // these are the exceptions
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
}

