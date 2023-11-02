#include "syscall.h"
#include "lib.h"


int32_t halt (uint8_t status) {
    printf("reached halt");
    return 0; 
}

int32_t execute (const uint8_t* command) {
    printf("reached execute");
    return 0;
}

int32_t read (uint32_t fd, void* buf, uint32_t nbytes) {
    printf("reached read");
    return 0;
}

int32_t write (uint32_t fd, const void* buf, uint32_t nbytes) {
    printf("reached write");
    return 0;
}

int32_t open(const uint8_t* filename) {
    printf("reached open");
    return 0;
}

int32_t close (int32_t fd) {
    printf("reached close");
    return 0;
}

