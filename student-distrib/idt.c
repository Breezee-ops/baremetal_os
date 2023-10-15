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
    for(i = 32; i < NUM_VEC; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].dpl = 0;
        idt[i].present = 0;
    }
    // TODO: fix this for the system calls, typically start at 0x80 (correct in doc)
    idt[0x80].reserved3 = 1;
    idt[0x80].present = 1;
    idt[0x80].dpl = 3;

    // set idt index and then the function pointer
}
