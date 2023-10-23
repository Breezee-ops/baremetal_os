#include "types.h"
#include "i8259.h"
#include "lib.h"

#define MAX_CHARS 127

#define L_SHIFT_HELD 0x2A
#define R_SHIFT_HELD 0x36
#define L_CTRL_HELD 0x1D
#define R_CTRL_HELD 
#define CAPS_HELD 0x3A
#define L_ALT_HELD 0x38
#define BACKSPACE_HELD 0x0E
#define TAB_HELD 0x0F
#define L_ARROW_HELD 0x4B
#define R_ARROW_HELD 0x4D
#define D_ARROW_HELD 0x50
#define U_ARROW_HELD 0x48


#define BACKSPACE_RAISE 0x8E
#define L_SHIFT_RAISE 0xAA
#define R_SHIFT_RAISE 0xB6
#define L_CTRL_RAISE  0x9D
#define R_CTRL_RAISE  
#define CAPS_RAISE  0xBA
#define L_ALT_RAISE 0xB8
#define TAB_RAISE 0x8F
#define L_ARROW_RAISE 0xCB
#define R_ARROW_RAISE 0xCD
#define D_ARROW_RAISE 0xD0
#define U_ARROW_RAISE 0xC8


void diverror() ; 
void debug() ; 
void nmi() ; 
void breakpoint() ; 
void overflow() ; 
void boundrange() ; 
void invalidop() ; 
void device_na() ; 
void doublefault() ; 
void invalidtss() ; 
void segment_na() ; 
void stack_seg() ; 
void general_protection() ; 
void page_fault() ; 
void floating_excep() ; 
void alignment_check() ; 
void machine_check() ; 
void smd() ; 
void coprocessor() ; // added
void keyboard_init(void);
void keyboard_handler(void);
int special_check(int key);
