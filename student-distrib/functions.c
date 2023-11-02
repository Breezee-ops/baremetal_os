#include "functions.h"
#include "lib.h"
#include "i8259.h"
#include "term.h"
#include "fs.h"
#include "paging.h"
#include "pcb.h"

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
        term_write(&printed_key, 1);
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

// system calls:
uint8_t exe_check(dentry_t* dirptr){
    uint8_t magic_num[4] = {0x7f, 0x45, 0x4c, 0x46};
    uint8_t buf[4];
    int i;
    file_read(dirptr->inode_num, 0, buf, 4);
    for(i = 0; i < 4; i++){
       if(buf[i] != magic_num[i]) return -1;
    }
    return 0;
}

int context_switch(uint32_t pid){
    uint32_t user_stack_ptr = 0x800000 - (pid * 8000);
    tss.esp0 = user_stack_ptr;
    uint32_t* user_esp = (uint32_t*) 0x7DE2900;
    asm volatile (" \n\
    movw %1, %%ds   \n\
    pushl %1        \n\
    pushl %3        \n\
    pushfl          \n\
    pushl %2        \n\
    pushl %0        \n\
    iret            \n\
    "
    ::"r" (eip), "r"(USER_DS), "r" (USER_CS), "r"(user_esp) : "cc"
    );
}

// TODO: this function takes a command; change this out
int32_t execute(const uint8_t* command){
    dentry_t dir = {
        .filename = "",      // Empty string, initializes all characters to 0 ('\0')
        .filetype = 0,       // 0 for uint32_t
        .inode_num = 0,      // 0 for uint32_t
        .reserved = {0}      // Initializes all elements of the reserved array to 0
    };
    uint8_t filename[FILENAME_LEN] = {0};
    int ind;
    for(ind = 0; ind < FILENAME_LEN && command[ind] != " "; ind++){
        filename[ind] = command[ind];
    }
    dentry_t* dirptr = &dir; 
    if(read_dentry_by_name((uint8_t*)filename, dirptr) != 0) return -1;
    if(exe_check(dirptr) != 0) return -1;
    uint8_t buffer[4];
    // dereference 8mb - 4kb for shell and 16kb for process and put the pcb struct type obj at the top of these two stacks.
    read_data(dirptr->inode_num, 24, (uint8_t*)&eip, 4);
    // cs is user_cs in inline assembly pushfl for eflags ds is set to user_ds
    int i;
    char j;

    uint32_t pid = find_pid();
    pcb_t* currpcb = pcb_init(pid);
    set_exe_page(pid);
    // read the executable into memory
    read_data(dirptr->inode_num, 0, (uint8_t*)0x08048000, 0x300000);
    currpcb->parent_pid = pid -1;
    uint32_t esp, ebp;
    asm volatile("\n\
    movl %%esp, %0  \n\
    movl %%ebp, %1  \n\
    " : "=r" (esp), "=r" (ebp) : : "cc");
    currpcb->esp = esp;
    currpcb->ebp = ebp;
    context_switch(pid);
    return 0;
}
