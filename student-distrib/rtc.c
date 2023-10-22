#include "rtc.h"
#include "lib.h"
#include "i8259.h"

// private vars used here
static unsigned long rtc_rate;
static unsigned long tally;
static unsigned long global_count;
static unsigned long return_flag_read;

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

    // set rtc to highest frequency possible that doesn't glitch in the emulation (1024)
    rtc_rate = 6;
    outb(0x8a, 0x70);
    outb(rtc_rate, 0x71);
    enable_irq((uint32_t) 8); // call the enable function to set the interrupt on the pic
    global_count = 0;
    
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
    if(global_count < tally){
        global_count ++;
    }
    else{
        // test_interrupts(); // whatever the rtc handler is supposed to do
        global_count = 0;
        return_flag_read = 1;
    }
    cli(); // protect from interrupts
    outb(0x0c, 0x70); // read contents into register (0x0c)
    inb(0x71); // throw contents away
    send_eoi((uint32_t)8); // send eoi so that we can finish servicing interrupt
    sti(); // re enable interrupts
}

void rtc_freq(int32_t freq){
    if(freq < base_freq){
        tally = base_freq / freq;
    }
}

int32_t rtc_open(const uint8_t* filename){
    rtc_freq(2);
    return 0;
}

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    return_flag_read = 0;
    while(!return_flag_read);
    return 0;
}

int32_t rtc_write((int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL) return -1;
    int32_t* freq = (int32_t*)buf;
    int32_t val = *freq;
    if(val & (val - 1) == 0) return -1; // not sure here
    cli();
    rtc_freq(val);
    sti();
    return 0;
}

int32_t rtc_close(int32_t fd){
    return 0;
}