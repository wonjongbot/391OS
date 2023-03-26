#include "filesystem.h"

void filesystem_init(const uint32_t fs_start) {
    _boot_block = *(boot_block*) fs_start;
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    uint32_t i;
    for (i = 0; i < _boot_block.dentry_count; i++) {
        uint32_t size = strlen((char*) fname);
        if (size > 32) size = 32; // Max name size
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

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    return -1;
}

int32_t filesystem_open(const uint8_t* filename) {
   return -1;
}
