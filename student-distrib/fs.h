#include "types.h"
#include "lib.h"

#ifndef _FS_H
#define _FS_H

#ifndef ASM

#define FILENAME_LEN 32
#define DIR_ENTRIES_LEN 63
#define BLOCK_SIZE 4096

typedef struct {
    char filename[FILENAME_LEN];//flag
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[24];
} dentry_t;

typedef struct {
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[52];
    dentry_t dir_entries[DIR_ENTRIES_LEN];
} boot_block_t;



typedef struct {
    uint32_t length;
    uint32_t data_block_num[1023];
} inode_t;

typedef uint8_t data_block_t;

uint32_t eip;
boot_block_t* boot_block;
inode_t* inode_blocks;
data_block_t* data_blocks;

void fs_init(uint32_t mod_start); 


uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry); 
//testing
//uint32_t read_dentry_by_name(const char* fname, dentry_t* dentry);

uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
uint32_t read_data(int32_t f_desc, int32_t offset, void* buf, int32_t length);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes); 
int32_t file_read(int32_t f_desc, int32_t offset, void* buf, int32_t length);
int32_t file_open(const uint8_t* file_name);
int32_t file_close(int32_t fd);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_read(int32_t f_desc, int32_t offset, void* buf, int32_t length); 
int32_t directory_open(const uint8_t* file_name); 
int32_t directory_close(int32_t fd); 

#endif /* ASM */

#endif /* _FS_H */

