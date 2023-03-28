#include "filesystem.h"
#include "lib.h"

/*
 *   filesys_init
 *   DESCRIPTION: initialize the file system.
 *   INPUTS: the pointer to the boot block
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void filesystem_init(const uint32_t fs_start) {
    _boot_block = *(boot_block*) fs_start;
    _inodes = (inode_t*) (fs_start + sizeof(boot_block));
    _data_blocks = (data_block*) (fs_start + sizeof(boot_block) + _boot_block.inode_count * sizeof(inode_t));
    //filearray[FILEARR_SIZE]  = (filed*)(fs_start + sizeof(boot_block) + _boot_block.inode_count * sizeof(inode));
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: search for the fname through the boot block and fill up the input dentry if found
 *   INPUTS: fname  --- the file name we want to search
 *           dentry --- the input dentry we wil fill up
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful
 *                 -1 if fail
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    if (fname == NULL || dentry == NULL) return -1;
    uint32_t i;
    uint32_t size = strlen((char*) fname);
    if (size > 32) return -1; // Max name size
    for (i = 0; i < _boot_block.dentry_count; i++) {
        if (strncmp((char*) fname, (char*) _boot_block.dentries[i].name, size) == 0) {
            *dentry = _boot_block.dentries[i];
            return 0;
        }
    }
    return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: search for the dentry index through the boot block and fill up the input dentry if found
 *   INPUTS: index  --- the  dentry index we want to search
 *           dentry --- the input dentry we wil fill up
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful
 *                 -1 if fail
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if (index >= _boot_block.dentry_count || dentry == NULL) return -1;

    *dentry = _boot_block.dentries[index];
    return 0;
}

/*
 *   read_data
 *   DESCRIPTION: read the data in the inode into the buf
 *   INPUTS: inode --- the index of the inode
 *           offset --- the data starting byte
 *           buf --- read the data into it
 *           length --- the size of data we want to read
 *   OUTPUTS: none
 *   RETURN VALUE: size of data we read
 *                 -1 if not successful
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length) {
    if (inode_idx > _boot_block.inode_count || buf == NULL) return -1;

    inode_t node = _inodes[inode_idx];

    uint32_t bytes_read = 0;

    uint32_t position;
    uint32_t data_block_index;
    uint32_t block_offset;
    for (position = offset; position < offset + length; position++) {
        //check if out of bound
        if (position >= node.length) return bytes_read;

        data_block_index = position >> _4kB_shift;
        block_offset = position - (data_block_index << _4kB_shift);
        data_block_index = node.data_block_nums[data_block_index];
        if (data_block_index >= _boot_block.data_block_count) return -1;
        //read into buffer
        buf[bytes_read++] = *((uint8_t*) _data_blocks[data_block_index].data + block_offset);

    }

    return bytes_read;
}

int32_t f_open(const uint8_t* filename) {
    return 0;
}

/*
 *   d_open
 *   DESCRIPTION: open the directory
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t d_open(const uint8_t* filename) {
    return 0;
}

/*
 * f_write
 *   DESCRIPTION: write the file
 *   INPUTS: fd; buffer; nbytes
 *   OUTPUTS: none
 *   RETURN VALUE:  just return -1
 *   SIDE EFFECTS: none
 */
int32_t f_write(int32_t fd, const void* buf, int32_t bytes) {
    return -1;
}

/*
 *   d_write
 *   DESCRIPTION:  return -1
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t d_write(int32_t fd, const void* buf, int32_t bytes) {
    return -1;
}

/*
 * f_close
 *   DESCRIPTION: close the file
 *   INPUTS: fd --- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE:  0 if successful
 *   SIDE EFFECTS: none
 */
int32_t f_close(int32_t fd) {
    return 0;
}

/*
 *   d_close
 *   DESCRIPTION: close the directory
 *   INPUTS: fd -- file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t d_close(int32_t fd) {
    return 0;
}

/*
 *   f_read
 *   DESCRIPTION: read the data of the file into the buffer
 *   INPUTS: fd --- file descriptor
 *           buf --- read data in it
 *           nbytes --- the size of the data we read
 *   OUTPUTS: none
 *   RETURN VALUE: the bytes of data we read
 *   SIDE EFFECTS: none
 */
int32_t f_read(int32_t fd, void* buf, int32_t bytes) {
    //check
    if (fd < 2 || fd > 7) {
        return -1;
    }
    if (buf == NULL || bytes < 0)
        return 0;
    //get the inode_idex and offset from the filearray acroding to fd
    uint32_t inode_idx = filearray[fd].inode_index;
    uint32_t offset = filearray[fd].file_position;
    //write into buffer
    uint32_t read_bytes = read_data(inode_idx, offset, buf, bytes);
    if (read_bytes == -1) {
        return -1;
    }
    //update the file_position
    filearray[fd].file_position += read_bytes;
    return read_bytes;
}

/*
 *   d_read
 *   DESCRIPTION: read the directory
 *   INPUTS: fd; buf; nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: the len of the read name
 *                 -1 if fail or read all in the current fd
 *
 *   SIDE EFFECTS: none
 */
int32_t d_read(int32_t fd, void* buf, int32_t bytes) {

    dentry_t dentry;

    //check
    if (fd < 2 || fd > 7) {
        return -1;
    }
    //check if read all file name in current fd
    if (filearray[fd].file_position == _boot_block.dentry_count) {
        return -1;
    }

    //read the diectory by the position of the file
    if (read_dentry_by_index(filearray[fd].file_position, &dentry) != 0) {
        return -1;
    }

    //clean buf
    /*for(i = 0; i < 32; i ++){
        ((int8_t*)buf)[i] = '\0';
    }*/
    //write into buffer
    strncpy((int8_t*) buf, (int8_t*) (&(dentry.name)), bytes);
    //update the file_position
    filearray[fd].file_position += 1;

    return strlen((int8_t*) (&(dentry.name)));
}
