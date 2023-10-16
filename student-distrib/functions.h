#include "types.h"
#include "i8259.h"
#include "lib.h"

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
