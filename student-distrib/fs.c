# include "fs.h"


void fs_init(uint32_t mod_start) {
    boot_block = (boot_block_t*)(mod_start); // start of boot block
    inode_blocks = (inode_t*)(boot_block + 1); // start of inodes
    data_blocks = (data_block_t*)(inode_blocks + boot_block->inode_count); // start of data blocks
}


uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    int i;

    for (i = 0; i < DIR_ENTRIES_LEN; i++) {
        if (strncmp((char*)boot_block->dir_entries[i].filename, (const char*)fname, FILENAME_LEN) == 0) {
            memcpy(dentry, &(boot_block->dir_entries[i]), sizeof(dentry_t));
            return 0;
        }
    }

    return -1;
}

uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if(index < 0 || index >= boot_block->dir_count) //flag linked to above
        return -1;

    memcpy(dentry, &(boot_block->dir_entries[index]), sizeof(dentry_t));
    return 0;
}

uint32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length) {
    int i;
    uint32_t bytes_read = 0;
    uint32_t* data2cpy;
    inode_t* inode2cpy = (inode_t*)(inode_blocks + inode_idx);

    if(offset > inode2cpy->length || inode_idx < 0 || inode_idx >= boot_block->inode_count)
        return -1;

    uint32_t start_block = offset / BLOCK_SIZE;
    uint32_t end_block = (offset + length) / BLOCK_SIZE;
    for(i = start_block; i < end_block; i++) {

        int l = 0;
        int r = BLOCK_SIZE;
        if(i == start_block) //offset left if at the start of read
            l = offset % BLOCK_SIZE;
        if(i == end_block - 1) //offset right if at the end of read
            r = (offset + length) % BLOCK_SIZE;
        

        for(l = l; l < r; l++) {
            data2cpy = (uint32_t*)(data_blocks + inode2cpy->data_block_num[i] * BLOCK_SIZE + l);
            memcpy(buf + bytes_read,data2cpy,1);
            bytes_read++;
        }

        if(i == inode2cpy->length - 1) {//if reach end of file before length then return 0
            return 0;
        }
    }

    return bytes_read;// success
}

uint32_t file_write(uint8_t file_name) {
    // read only file system
    return -1; 
}

uint32_t file_read(uint32_t f_desc, uint32_t* offset, uint8_t* buf, uint32_t length) {
    
    // sanity check on file system init
    if (boot_block == NULL || buf == NULL) return -1; 

    // result -> read data function call
    int32_t ret = read_data(f_desc, *offset, buf, length); 

    // add bytes read to offset
    if (ret < 0) { 
        return -1; 
    } else {
        *offset += ret; // new offset value to be used in testing
        return ret; 

    }    
}

uint32_t file_open(const uint8_t* file_name) {
    return 0; 
}

uint32_t file_close(const uint8_t* file_name) {
    return 0; 
}

uint32_t directory_write() {
    //read only file system
    return -1; 
}

uint32_t directory_read(uint32_t* offset, uint8_t* buf, uint32_t length) {

    dentry_t* dir_entry; 

    // sanity check on file system init, offset, index, and buffer
    if (*offset > DIR_ENTRIES_LEN || boot_block == NULL || buf == NULL || length < 0) {
        return -1; 
    }

    // use passed in offset to retrieve the dentry pointer
    dir_entry = &(boot_block->dir_entries[*offset]);

    // sanity check on length of filename
    // memcopy filename 
    strncpy((char*)buf, (char*)dir_entry->filename, FILENAME_LEN);

    if (strlen((char*)dir_entry->filename) > FILENAME_LEN) {
        *offset += 1; // go to the next file
        return FILENAME_LEN; 
    } else if (strlen((char*)dir_entry->filename) > 0){
        *offset += 1; // go to the next file
        return strlen((char*)dir_entry->filename); 
    } else {
        return 0; 
    }
    
}

uint32_t directory_open(const uint8_t* file_name) {
    return 0; 
}
uint32_t directory_close(const uint8_t* file_name) {
    return 0; 
}


