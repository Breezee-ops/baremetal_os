#include "functions.h"
#include "lib.h"
#include "i8259.h"

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

void keyboard_init(void){
    enable_irq(1);
}

void keyboard_handler(void){
    cli();
    uint8_t key = inb(0x60);
    putc(key);
    send_eoi(1);
    sti();
}

