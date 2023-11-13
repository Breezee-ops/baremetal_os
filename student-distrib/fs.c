# include "fs.h"

/* fs_init
 * 
 *  Initialize file system by setting relevent memory addresses
 * Inputs: mod_start - the starting address of fs memory
 * Outputs: None
 * Side Effects: Initializes global pointers to file system data structures
 * Coverage: Boot block, inodes, and data blocks
 */
void fs_init(uint32_t mod_start) {
    boot_block = (boot_block_t*)(mod_start); // start of boot block
    inode_blocks = (inode_t*)(boot_block + 1); // start of inodes
    data_blocks = (data_block_t*)(inode_blocks + boot_block->inode_count); // start of data blocks
}

/* read_dentry_by_name
 * 
 *  Find directory entry by name
 * Inputs: fname - filename we want to search for
 *         dentry - dentry pointer to store result
 * Outputs: 0 if the dentry is found, -1 if not found
 * Side Effects: None
 */
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

/* read_dentry_by_index
 * 
 *  Finds dentry by searching via index
 * Inputs: index - index we are searching for
 *         dentry - dentry pointer to store result
 * Outputs: 0 if the dentry is found, -1 if not found
 * Side Effects: None
 */
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if(index < 0 || index >= boot_block->dir_count) //flag linked to above
        return -1;

    memcpy(dentry, &(boot_block->dir_entries[index]), sizeof(dentry_t));
    return 0;
}

/* read_data 
 * 
 *  Read data from specifiec offset into buffer
 * Inputs: inode_idx - index of file inode
 *         offset - starting offset
 *         buf - buffer pointer to store the read data
 *         length - length of data to read
 * Outputs: The number of bytes read if successful, -1 on error
 * Side Effects: None
 */
uint32_t read_data(int32_t f_desc, int32_t offset, void* buf, int32_t length) {
    int i;
    uint32_t inode_idx = (uint32_t)f_desc;
    uint32_t bytes_read = 0;
    uint32_t* data2cpy;
    inode_t* inode2cpy = (inode_t*)(inode_blocks + inode_idx);
    if(inode_idx < 0 || inode_idx >= boot_block->inode_count)
        return -1;

    if(offset >= inode2cpy->length) return 0;

    uint32_t start_block = offset / BLOCK_SIZE;
    uint32_t end_block = (offset + length) / BLOCK_SIZE;
    for(i = start_block; i <= end_block; i++) {
        int l = 0;
        int r = BLOCK_SIZE;
        if(i == start_block) //offset left if at the start of read
            l = offset % BLOCK_SIZE;
        if(i == end_block ) //offset right if at the end of read
            r = (offset + length) % BLOCK_SIZE;
        

        for(l = l; l < r; l++) {
            data2cpy = (uint32_t*)(data_blocks + inode2cpy->data_block_num[i] * BLOCK_SIZE + l);
            memcpy((uint8_t*)buf + bytes_read,data2cpy,1);
            bytes_read++;
            if(bytes_read == length || l >= inode2cpy->length) {
                // if(f_desc == 38) {
                //     return 187;
                // } else {
                return bytes_read;
                // }
            }
        }

        if(i == inode2cpy->length - 1) {//if reach end of file before length then return 0
            return 0;
        }
    }

    return bytes_read;// success
}

/* file_write 
 * 
 *      Write to a file
 * Inputs: file_name - the name of the file
 * Outputs: -1
 * Side Effects: None
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    // read only file system
    return -1; 
}

/* file_read 
 * 
 *   Read data from with a specified offset, perform sanity checks 
 * Inputs: fd - the file descriptor
 *         offset - the starting offset in the file
 *         buf - a pointer to the buffer to store the read data
 *         length - the length of data to read
 * Outputs: The number of bytes read if successful, -1 on error
 * Side Effects: None
 */
int32_t file_read(int32_t fd, int32_t* offset, void* buf, int32_t length) {
    
    // sanity check on file system init
    if (boot_block == NULL || buf == NULL || offset < 0) return -1; 

    // result -> read data function call
    int32_t ret = read_data(fd, *offset, buf, length); 

    int32_t temp = *offset + ret;
    *offset = temp;

    if (ret < 0) { 
        return -1; 
    } else {
        return ret; 
    }    
}

/* file_open 
 * 
 *  Opens a file
 * Inputs: file_name - the name of the file
 * Outputs: 0
 * Side Effects: None
 */
int32_t file_open(const uint8_t* file_name) {
    return 0; 
}

/* file_close 
 * 
 *  Close a file
 * Inputs: file_name - the name of the file
 * Outputs: 0
 * Side Effects: None
 */
int32_t file_close(int32_t fd) {
    return 0; 
}

/* directory_write 
 * 
 *  Write to the directory
 * Inputs: None
 * Outputs: -1
 * Side Effects: None
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) {
    //read only file system
    return -1; 
}

/* directory_read 
 * 
 *  Read from directory at offset value and copy into buffer
 * Inputs: offset -  directory entry offset
 *         buf - buffer pointer to store the directory entry
 * Outputs: The number of bytes read if successful, -1 on error
 * Side Effects: None
 */
int32_t directory_read(int32_t fd, int32_t* offset, void* buf, int32_t length) {

    dentry_t* dir_entry; 
    uint32_t len;
    // sanity check on file system init, offset, index, and buffer
    if (*offset > DIR_ENTRIES_LEN-1 || boot_block == NULL || buf == NULL) {
        return -1; 
    }

    // use passed in offset to retrieve the dentry pointer
    dir_entry = &(boot_block->dir_entries[*offset]);

    // strncpy filename 
    strncpy((int8_t*) buf, (int8_t*) dir_entry->filename, FILENAME_LEN);
    len = strlen((char*)dir_entry->filename);

    if (len == 0) {
        return 0;
    } else {
        int32_t temp = *offset + 1;
        *offset = temp;
    }

    if (len > FILENAME_LEN) {
        return FILENAME_LEN; 
    } else if (len > 0){
        return len; 
    } else {
        return -1; 
    }
}


/* directory_open
 * 
 *  Opens a directory
 * Inputs: file_name - the name of the directory
 * Outputs: 0
 * Side Effects: None
 */
int32_t directory_open(const uint8_t* file_name) {
    return 0; 
}


/* directory_close
 * 
 *   Close a directory
 * Inputs: file_name - the name of the directory
 * Outputs: 0
 * Side Effects: None
 */
int32_t directory_close(int32_t fd) {
    return 0; 
}
