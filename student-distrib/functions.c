#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"

// define C functions for setting all of the exceptions

void diverror() {
    printf("Divide by zero error"); 
    while(1); 
}
void debug() {
    printf("Debug Exception error"); 
    while(1); 
}
void nmi() {
    printf("NMI Interrupt"); 
    while(1); 
}
void breakpoint() {
    printf("breakpoint Interrupt"); 
    while(1); 
}
void overflow() {
    printf("overflow Interrupt"); 
    while(1); 
}
void boundrange() {
    printf("bound range Interrupt"); 
    while(1); 
}
void invalidop() {
    printf("invalid opcode error"); 
    while(1); 
}
void device_na() {
    printf("Device not available error"); 
    while(1); 
}
void doublefault() {
    printf("Double fault Interrupt"); 
    while(1); 
}
void invalidtss() {
    printf("Invalid TSS Interrupt"); 
    while(1); 
}
void segment_na() {
    printf("segment not available"); 
    while(1); 
}
void stack_seg() {
    printf("stack segment fault"); 
    while(1); 
}
void general_protection() {
    printf("general protection error"); 
    while(1); 
}
void page_fault() {
    printf("page fault error"); 
    while(1); 
}
void floating_excep() {
    printf("floation point exception"); 
    while(1); 
}
void alignment_check() {
    printf("alignment exception"); 
    while(1); 
}
void machine_check() {
    printf("machine check exception"); 
    while(1); 
}
void smd() {
    printf("smd floating exception"); 
    while(1); 
}
void coprocessor() {
    printf("coprocessor exception"); 
    while(1); 
}

<<<<<<< Updated upstream
=======

//enable the irq line 1 on the master to accept keyboard input
>>>>>>> Stashed changes
void keyboard_init(void){
    
    enable_irq(1);
}
    //initialize all special keys status vars
    int shift_held = 0;
    int backspace_held = 0;
    int ctrl_held = 0;
    int alt_held = 0;
    int capslock_on = 0;

<<<<<<< Updated upstream
char keymap[128] =
=======
//keymap to translate PS/2 Scancode (set 1) to ASCII characters
char lower_keymap[128] =
>>>>>>> Stashed changes
{
    0,  
    27, 
    '1', 
    '2', 
    '3', 
    '4', 
    '5', 
    '6', 
    '7', 
    '8', 
    '9', 
    '0', 
    '-', 
    '=', 
    '\b', 
    '\t',  
    'q', 
    'w', 
    'e', 
    'r', 
    't', 
    'y', 
    'u', 
    'i', 
    'o', 
    'p', 
    '[', 
    ']', 
    '\n',
    0,   
  'a', 
  's', 
  'd', 
  'f', 
  'g', 
  'h', 
  'j', 
  'k', 
  'l', 
  ';',
 '\'', 
 '`',   
 0,    
 '\\', 
 'z', 
 'x', 
 'c', 
 'v', 
 'b', 
 'n',      
  'm', 
  ',', 
  '.', 
  '/', 
  '*',
  ' ',  
};
<<<<<<< Updated upstream

=======
char upper_keymap[128] =
{
    0,  
    27, 
    '1', 
    '2', 
    '3', 
    '4', 
    '5', 
    '6', 
    '7', 
    '8', 
    '9', 
    '0', 
    '-', 
    '=', 
    '\b', 
    '\t',  
    'Q', 
    'W', 
    'E', 
    'R', 
    'T', 
    'Y', 
    'U', 
    'I', 
    'O', 
    'P', 
    '[', 
    ']', 
    '\n',
    0,   
    'A', 
    'S', 
    'D', 
    'F', 
    'G', 
    'H', 
    'J', 
    'K', 
    'L', 
    ';',
    '\'',
    '`',   
    0,    
    '\\', 
    'Z', 
    'X', 
    'C', 
    'V', 
    'B', 
    'N',      
    'M', 
    ',',
    '.',
    '/',
    '*',
    ' ',  
};
/* keyboard_handler
 * 
 * takes input from keyboard and converts the scancode to an ascii character
 * Inputs: inb from keyboard port 0x60
 * Outputs: 
 * Side Effects: None
 * Coverage: Keyboard I/O
 */
>>>>>>> Stashed changes
void keyboard_handler(void){
    cli();
    uint8_t key = inb(0x60);
<<<<<<< Updated upstream
    if(key & 0x80){
=======

    unsigned char printed_key; 

    if(special_check(key)){
        send_eoi(1);
        sti();
        return;
    }

    if((shift_held == 1 && capslock_on == 0) || (shift_held == 0 && capslock_on == 1)){
        printed_key = upper_keymap[key];
    }
    else if((shift_held == 1 && capslock_on == 1) || (shift_held == 0 && capslock_on == 0)){
        printed_key = lower_keymap[key];
    }
    

    //if the key is just being released or a backspace, ignore it
    if(key & 0x80 || backspace_held == 1){
>>>>>>> Stashed changes
    }
    else{
        putc(printed_key);
    }
    send_eoi(1);
    sti();
}

int special_check(int key){
    switch(key){
        case L_SHIFT_HELD:
            case R_SHIFT_HELD:
                shift_held = 1;
                return 1;
        case L_ALT_HELD:
            alt_held = 1;
            return 1;
        case L_CTRL_HELD:
            ctrl_held = 1;
        case BACKSPACE_HELD:
            uh_oh_backspace();
            backspace_held = 1;
            return 0;
        case CAPS_HELD:
            if(capslock_on == 0){
                capslock_on = 1;
            }
            else{
                capslock_on = 0;
            }
        case L_SHIFT_RAISE:
            case R_SHIFT_RAISE:
            shift_held = 0;
            return 1;
        case L_ALT_RAISE:
            alt_held = 0;
            return 1;
        case L_CTRL_RAISE:
            ctrl_held = 0;
            return 1;
        case BACKSPACE_RAISE:
            backspace_held = 0;
            return 1;

    }
    return 0;
}

