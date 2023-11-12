#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"


static volatile int enter_flag = 0;

int get_flag(){
    return enter_flag;
}

void set_flag(int flag){
    enter_flag = flag;
}

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

//enable the irq line 1 on the master to accept keyboard input
void keyboard_init(void){
    enable_irq(1);
}
    //initialize all special keys status vars
    int shift_held = 0;
    int backspace_held = 0;
    int ctrl_held = 0;
    int alt_held = 0;
    int capslock_on = 0;
    int tab_held;
    int arrow_held = 0;

//lowercase keymap to translate PS/2 Scancode (set 1) to ASCII characters
char lower_keymap[] =
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
  ' '  
};
//uppercase keymap when SHIFT and CAPSLOCK
char upper_keymap[] =
{
    0,
    27,
    '!',     
    '@',     
    '#',     
    '$',    
    '%',     
    '^',     
    '&',    
    '*',      
    '(',     
    ')',    
    '_',    
    '+',    
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
    '{',    
    '}',    
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
    ':',     
    '\"',    
    '~',     
    0,
    '|',     
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<',     
    '>',    
    '?',     
    '*',     
    ' '
};
/* keyboard_handler
 * 
 * takes input from keyboard and converts the scancode to an ascii character
 * Inputs: inb from keyboard port 0x60
 * Outputs: 
 * Side Effects: None
 * Coverage: Keyboard I/O
 */
void keyboard_handler(void){
    //begin critical section stop all interrupts
    cli();
    //read pressed key from keyboard
    uint8_t key = inb(0x60);
    enter_flag = 0;
    unsigned char printed_key; 

    //check if a key that alters keypress is pressed
    if(special_check(key)){
        send_eoi(1);
        sti();
        return;
    }

    //if CTRL + L is pressed clear terminal
    if(ctrl_held == 1 && key == 0x26){
        term_clear();
    }
    if(ctrl_held == 1 && key == 0x2E){

        printf("Control C Pressed"); 

    }
    if(key == 28){
        enter_flag = 1;
    }
    //logic to handle if we should print uppercase letter
    if((shift_held == 1 && capslock_on == 0) || (shift_held == 0 && capslock_on == 1)){
        printed_key = upper_keymap[key];
    }
    //logic to handle if we should print lowercase letter
    else if((shift_held == 1 && capslock_on == 1) || (shift_held == 0 && capslock_on == 0)){
        printed_key = lower_keymap[key];
    }
    

    //if the key is just being released, ctrl, an arrow key, or a function key ignore input
    if(key & 0x80 || backspace_held == 1 || tab_held == 1 || ctrl_held == 1 || arrow_held == 1 || ((key  >= 0x3B) && (key <= 0x44)) || (key == 0x57) || (key == 0x58)){
    }
    //otherwise call term_write to add to buffer and print to screen
    else{
        keyboard_read(&printed_key);
    }
    //done with interrupt
    send_eoi(1);
    //resume interrupts
    sti();
}

/* special_check
 * 
 * checks if key pressed is a special key that alters typing
 * Inputs: inb from keyboard port 0x60
 * Outputs: Flag if special key is held
 * Side Effects: None
 * Coverage: Keyboard I/O
 */
int special_check(int key){
    switch(key){
        case L_SHIFT_HELD:                  //same response for shift keys
            case R_SHIFT_HELD:
                shift_held = 1;
                return 1;
        case L_ALT_HELD:
            alt_held = 1;
            return 1;
        case L_CTRL_HELD:
            ctrl_held = 1;
            return 1;
        case BACKSPACE_HELD:                //special case for backspace
            uh_oh_backspace();
            backspace_held = 1;
            return 1;
        case CAPS_HELD:                     //if capslock is on, turn off vice versa
            if(capslock_on == 0){
                capslock_on = 1;
            }
            else{
                capslock_on = 0;
            }
        case TAB_HELD:                      //special input for tab
            tabitha();
            tab_held = 1;
            return 1;
        case L_ARROW_HELD:                  //same response for all arrow keys
            case R_ARROW_HELD:
                case U_ARROW_HELD:
                    case D_ARROW_HELD:
                        arrow_held = 1;                      
                        return 1;
        

        case L_SHIFT_RAISE:                 //same response for both shift keys
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
        case TAB_RAISE:
            tab_held = 0;
            return 1;
        case L_ARROW_RAISE:                 //same response for release
            case R_ARROW_RAISE:
                case U_ARROW_RAISE:
                    case D_ARROW_RAISE:
                        arrow_held = 0;                      
                        return 1;
    }
    return 0;
}
