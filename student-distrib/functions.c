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


struct ScanCodeMap scanCodeToCharMap[] = {
    {0x01, 0x1B},   // 0x01 escape pressed
    {0x02, '1'},    // 0x02 1 pressed
    {0x03, '2'},    // 0x03 2 pressed
    {0x04, '3'},    // 0x04 3 pressed
    {0x05, '4'},    // 0x05 4 pressed
    {0x06, '5'},    // 0x06 5 pressed
    {0x07, '6'},    // 0x07 6 pressed
    {0x08, '7'},    // 0x08 7 pressed
    {0x09, '8'},    // 0x09 8 pressed
    {0x0A, '9'},    // 0x0A 9 pressed
    {0x0B, '0'},    // 0x0B 0 (zero) pressed
    {0x0C, '-'},    // 0x0C - pressed
    {0x0D, '='},    // 0x0D = pressed
    {0x0E, '\b'},   // 0x0E backspace pressed
    {0x0F, '\t'},   // 0x0F tab pressed
    {0x10, 'q'},    // 0x10 Q pressed
    {0x11, 'w'},    // 0x11 W pressed
    {0x12, 'e'},    // 0x12 E pressed
    {0x13, 'r'},    // 0x13 R pressed
    {0x14, 't'},    // 0x14 T pressed
    {0x15, 'y'},    // 0x15 Y pressed
    {0x16, 'u'},    // 0x16 U pressed
    {0x17, 'i'},    // 0x17 I pressed
    {0x18, 'o'},    // 0x18 O pressed
    {0x19, 'p'},    // 0x19 P pressed
    {0x1A, '['},    // 0x1A [ pressed
    {0x1B, ']'},    // 0x1B ] pressed
    {0x1C, '\n'},   // 0x1C enter pressed
    {0x1D, 0x00},   // 0x1D left control pressed
    {0x1E, 'a'},    // 0x1E A pressed
    {0x1F, 's'},    // 0x1F S pressed
    {0x20, 'd'},    // 0x20 D pressed
    {0x21, 'f'},    // 0x21 F pressed
    {0x22, 'g'},    // 0x22 G pressed
    {0x23, 'h'},    // 0x23 H pressed
    {0x24, 'j'},    // 0x24 J pressed
    {0x25, 'k'},    // 0x25 K pressed
    {0x26, 'l'},    // 0x26 L pressed
    {0x27, ';'},    // 0x27 ; pressed
    {0x28, '\''},   // 0x28 ' (single quote) pressed
    {0x29, '`'},    // 0x29 ` (back tick) pressed
    {0x2A, 0x00},   // 0x2A left shift pressed
    {0x2B, '\\'},   // 0x2B \ pressed
    {0x2C, 'z'},    // 0x2C Z pressed
    {0x2D, 'x'},    // 0x2D X pressed
    {0x2E, 'c'},    // 0x2E C pressed
    {0x2F, 'v'},    // 0x2F V pressed
    {0x30, 'b'},    // 0x30 B pressed
    {0x31, 'n'},    // 0x31 N pressed
    {0x32, 'm'},    // 0x32 M pressed
    {0x33, ','},    // 0x33 , pressed
    {0x34, '.'},    // 0x34 . pressed
    {0x35, '/'},    // 0x35 / pressed
    {0x36, 0x00},   // 0x36 right shift pressed
    {0x37, '*'},    // 0x37 (keypad) * pressed
    {0x38, 0x00},   // 0x38 left alt pressed
    {0x39, ' '},    // 0x39 space pressed
    {0x3A, 0x00},   // 0x3A CapsLock pressed
    {0x3B, 0x00},   // 0x3B F1 pressed
    {0x3C, 0x00},   // 0x3C F2 pressed
    {0x3D, 0x00},   // 0x3D F3 pressed
    {0x3E, 0x00},   // 0x3E F4 pressed
    {0x3F, 0x00},   // 0x3F F5 pressed
    {0x40, 0x00},   // 0x40 F6 pressed
    {0x41, 0x00},   // 0x41 F7 pressed
    {0x42, 0x00},   // 0x42 F8 pressed
    {0x43, 0x00},   // 0x43 F9 pressed
    {0x44, 0x00},   // 0x44 F10 pressed
    {0x45, 0x00},   // 0x45 NumberLock pressed
    {0x46, 0x00},   // 0x46 ScrollLock pressed
    {0x47, '7'},    // 0x47 (keypad) 7 pressed
    {0x48, '8'},    // 0x48 (keypad) 8 pressed
    {0x49, '9'},    // 0x49 (keypad) 9 pressed
    {0x4A, '-'},    // 0x4A (keypad) - pressed
    {0x4B, '4'},    // 0
}

void keyboard_handler(void){
    //uint8_t key = inb(0x60);
    uint8_t key = inb(0x60);
    if(key == 0x08){
        putc('7');
    }
    else{
        putc('#');
    }
    send_eoi(1);
}

