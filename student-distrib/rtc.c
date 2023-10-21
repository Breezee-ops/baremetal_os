#include "rtc.h"
#include "lib.h"
#include "i8259.h"


/* init_rtc
 * 
 * initializes the rtc by turning on its interrupt on the pic
 * Inputs: None
 * Outputs: none
 * Side Effects: opens interupt 8 on the pic
 * Coverage: allow the rtc to send interrupts
 * Files: i8259.c/h
 */
void init_rtc() {
    outb(0x8b, 0x70); // 0x8b; status register B. 0x70 disable NMI 
    char prev = inb(0x71); // read value at reg b
    outb(0x8b, 0x70); // set index
    outb(prev | 0x40, 0x71); // return the flag values read by oring and writing to port
    enable_irq((uint32_t) 8); // call the enable function to set the interrupt on the pic
}
/* init_rtc
 * 
 * initializes the rtc by turning on its interrupt on the pic
 * Inputs: None
 * Outputs: none
 * Side Effects: opens interupt 8 on the pic
 * Coverage: allow the rtc to send interrupts
 * Files: i8259.c/h
 */
void rtc_handler(){
    //test_interrupts();
    cli(); // protect from interrupts
    outb(0x0c, 0x70); // read contents into register (0x0c)
    inb(0x71); // throw contents away
    send_eoi((uint32_t)8); // send eoi so that we can finish servicing interrupt
    sti(); // re enable interrupts
}

