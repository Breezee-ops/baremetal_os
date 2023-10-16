#include "rtc.h"
#include "lib.h"
#include "i8259.h"

void init_rtc() {
    outb(0x8b, 0x70);
    char prev = inb(0x71);
    outb(0x8b, 0x70);
    outb(prev | 0x40, 0x71);
    enable_irq((uint32_t) 8);
}
// copy paste osdev
void rtc_handler(){
    test_interrupts();
    cli();
    outb(0x0c, 0x70);
    inb(0x71);
    send_eoi((uint32_t)8);
    sti();
}

