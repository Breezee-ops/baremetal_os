// #include "types.h"
// #include "fs.h"
// #include "x86_desc.h"

// #ifndef _PCB_H
// #define _PCB_H

// #ifndef ASM

// typedef struct {
//     int32_t (*open)(const uint8_t* filename);
// 	int32_t (*close)(int32_t fd);
// 	int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
// 	int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
// } file_operation_t;

// typedef struct {
//     file_operation_t* file_operations;
//     inode_t* inode;
//     uint32_t file_position;
//     uint32_t flags;
// } fd_t;

// typedef struct {
//     fd_t fda[8];//file descriptor array
//     uint32_t esp;
//     uint32_t ebp;
//     uint32_t pid;
//     uint32_t parent_pid;
//     char args[128];
//     //... fill in as needed
// } pcb_t;

// int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
// int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
// pcb_t* pcb_init(uint32_t pid);
// pcb_t* get_pcb_ptr(uint32_t pid);
// uint32_t find_pid();
// uint32_t free_pid(uint32_t pid);
// #endif /* ASM */

// #endif /* _PCB_H */
