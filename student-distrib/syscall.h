#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "fs.h"
#include "paging.h"
#include "x86_desc.h"

typedef struct {
	int32_t (*read)(int32_t fd, int32_t* offset, void* buf, int32_t length);
	int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
	int32_t (*close)(int32_t fd);
} file_operation_t;

typedef struct {
    file_operation_t* file_operations;
    int32_t inode_num;
    int32_t file_position;
    uint32_t flags;
} fd_t;

typedef struct {
    fd_t fda[8];//file descriptor array
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t pid;
    uint32_t parent_pid;
    char args[128];
    //... fill in as needed
} pcb_t;

volatile pcb_t* cur_pcb_ptr;
volatile pcb_t pcbarr[6];

// int32_t terminal_read(int32_t fd, unsigned char* buf, int32_t nbytes);
// int32_t terminal_write(int32_t fd, const unsigned char* buf, int32_t nbytes);
int32_t shell_execute();
pcb_t* pcb_init(uint32_t pid, uint32_t parent_pid);
pcb_t* get_pcb_ptr(uint32_t pid);
int32_t find_pid();
uint32_t free_pid(uint32_t pid);

extern void syscall_handler_asm();

int context_switch(uint32_t pid);
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
void shell_exec();
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename); 
int32_t close (int32_t fd);  
int32_t vidmap (uint8_t** screen_start);
int32_t getargs(uint8_t* buf, int32_t nbytes);
void user_context_switch();
int find_fda_idx();
int latest_pid();
void save_stack();

#endif

