#include "rtc.h"
#include "lib.h"
#include "i8259.h"

// private vars used here
static unsigned long rtc_rate;
static unsigned long tally;
static unsigned long global_count;
static volatile unsigned long return_flag_read;

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
    int val = 2;
    int* ptr = &val;
    rtc_write(0, ptr, 4);
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

/* rtc_freq
 * 
 * sets the rtc to any frequency given
 * Inputs: freq
 * Outputs: none
 * Side Effects: sets the tally global variable
 * Coverage: reestricts virtual rtc to the speed mentioned
 * Files: i8259.c/h
 */
void rtc_freq(int32_t freq){
    if(freq <= base_freq){
        tally = base_freq / freq;
    }
}

/* rtc_open
 * 
 * sets rtc frequency to 2hz as per addendum
 * Inputs: ignored
 * Outputs: 0
 * Side Effects: sets global var tally
 * Coverage: reestricts virtual rtc to the speed mentioned
 * Files: i8259.c/h
 */
int32_t rtc_open(const uint8_t* filename){
    rtc_freq(2);
    return 0;
}

/* rtc_read
 * 
 * only creates an interrupt when we register an rtc inteprrput at the frequency we set it to
 * Inputs: ignored
 * Outputs: none
 * Side Effects: sets and checks return_flag_read global variable
 * Coverage: creates rtc functionality
 * Files: i8259.c/h
 */

int32_t rtc_read(int32_t f_desc, int32_t offset, void* buf, int32_t length){
    return_flag_read = 0;
    while(!return_flag_read){};
    test_interrupts();
    return 0;
}

/* rtc_write
 * 
 * sets the rtc to any frequency given
 * Inputs: ignored and buf
 * Outputs: none
 * Side Effects: sets the tally global variable
 * Coverage: restricts virtual rtc to the speed mentioned inside buffer
 * Files: i8259.c/h
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL) return -1;
    int32_t* freq = (int32_t*)buf;
    int32_t val = *freq;
    if((val & (val - 1)) != 0) return -1; // checks val to see if it's a power of two. essentially checks if there is only one 1 in the binary of val.
    cli();
    rtc_freq(val);
    sti();
    return 0;
}

// does nothing
int32_t rtc_close(int32_t fd){
    return 0;
}
