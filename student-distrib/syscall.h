#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

extern void syscall_handler_asm();

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (uint32_t fd, void* buf, uint32_t nbytes);
int32_t write (uint32_t fd, const void* buf, uint32_t nbytes);
int32_t open(const uint8_t* filename); 
int32_t close (int32_t fd);  
#endif

