#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include ""

extern void init_idt(idt_desc_t* idt);
extern void enable_irq(uint32_t irq_num); 
extern void disable_irq(uint32_t irq_num); 
extern void send_eoi(uint32_t irq_num); 

#endif