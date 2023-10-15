#include "functions.h"

// define C functions for setting all of the exceptions

void DIVZERO() {
    printf("Divide by zero error"); 
    while(0); 
}
void DEBUG_EX() {
    printf("Debug Exception error"); 
    while(0); 
}
void NMI_INT() {
    printf("NMI Interrupt"); 
    while(0); 
}
