#include "rtc.h"
#include "lib.h"

void init_rtc() {
    cli();
    outb(0x8b, 0x70);
    char prev = innb(0x71);
    outb(0x8b, 0x70);
    outb(prev | 0x40, 0x71);
    sti();
    enable_irq((uint32_t) 8);
}

void rtc_handler(){
    cli();
    outb(0x0c, 0x70);
    inb(0x71);
    sti();
}