#include "functions.h"

// define C functions for setting all of the exceptions

void diverror() {
    printf("Divide by zero error"); 
    while(0); 
}
void debug() {
    printf("Debug Exception error"); 
    while(0); 
}
void nmi() {
    printf("NMI Interrupt"); 
    while(0); 
}
void breakpoint() {
    printf("breakpoint Interrupt"); 
    while(0); 
}
void overflow() {
    printf("overflow Interrupt"); 
    while(0); 
}
void boundrange() {
    printf("bound range Interrupt"); 
    while(0); 
}
void invalidop() {
    printf("invalid opcode error"); 
    while(0); 
}
void device_na() {
    printf("Device not available error"); 
    while(0); 
}
void doublefault() {
    printf("Double fault Interrupt"); 
    while(0); 
}
void invalidtss() {
    printf("Invalid TSS Interrupt"); 
    while(0); 
}
void segment_na() {
    printf("segment not available"); 
    while(0); 
}
void stack_seg() {
    printf("stack segment fault"); 
    while(0); 
}
void general_protection() {
    printf("general protection error"); 
    while(0); 
}
void page_fault() {
    printf("page fault error"); 
    while(0); 
}
void floating_excep() {
    printf("floation point exception"); 
    while(0); 
}
void alignment_check() {
    printf("alignment exception"); 
    while(0); 
}
void machine_check() {
    printf("machine check exception"); 
    while(0); 
}
void smd() {
    printf("smd floating exception"); 
    while(0); 
}
void coprocessor() {
    printf("coprocessor exception"); 
    while(0); 
}