#include "filesystem.h"

void filesystem_init(const uint32_t fs_start) {
    _boot_block = *(boot_block*) fs_start;
    _inodes = (inode*) (fs_start + sizeof(boot_block));
    _data_blocks = (data_block*) (fs_start + sizeof(boot_block) + _boot_block.inode_count * sizeof(inode));
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
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

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if (index >= _boot_block.dentry_count) return -1;

    *dentry = _boot_block.dentries[index];
    return 0;
}

int32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length) {
    if (inode_idx > DENTRY_MAX) return -1;

    inode node = _inodes[inode_idx];

    uint32_t bytes_read = 0;

    uint32_t position;
    uint32_t data_block_index;
    uint32_t block_offset;
    for (position = offset; position < offset + length; position++) {
        if (position >= node.length) return bytes_read;

        data_block_index = position >> _4kB_shift;
        block_offset = position - (data_block_index << _4kB_shift);
        data_block_index = node.data_block_nums[data_block_index];
        if (data_block_index >= _boot_block.data_block_count) return -1;

        if (position < offset + length) {
            buf[bytes_read++] = *((uint8_t*) _data_blocks[data_block_index].data + block_offset);
        }
    }

    return bytes_read;
}

int32_t filesystem_open(const uint8_t* filename) {
   return -1;
}
