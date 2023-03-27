// Defines functions to interact with

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"

#define DENTRY_MAX              63
#define _32B                    (1 << 5)
#define _4kB_shift              12
#define _4kB                    (1 << _4kB_shift)
#define DATA_BLOCK_MAX          1023
#define DENTRY_RESERVED_B       24
#define BOOT_BLOCK_RESERVED_B   52
#define FILEARR_SIZE            8

typedef struct dentry_t {
    uint8_t name[_32B];
    uint32_t file_type;
    uint32_t inode;
    uint8_t reserved[DENTRY_RESERVED_B];
} dentry_t;

typedef struct boot_block {
    uint32_t dentry_count;
    uint32_t inode_count;
    uint32_t data_block_count;
    uint8_t reserved[BOOT_BLOCK_RESERVED_B];
    dentry_t dentries[DENTRY_MAX];
} boot_block;

typedef struct inode {
    uint32_t length;
    uint32_t data_block_nums[DATA_BLOCK_MAX];
} inode;

typedef struct data_block {
    uint8_t data[_4kB];
} data_block;

typedef struct file_ops{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t *filename);
    int32_t (*close)(int32_t fd);
}file_ops;
typedef struct filed{
    file_ops* ops;
    uint32_t inode_index;
    uint32_t file_position;
    uint32_t flags;
}filed;

boot_block _boot_block;
inode* _inodes;
data_block* _data_blocks;
filed filearray[FILEARR_SIZE];

void filesystem_init(const uint32_t fs_start);


int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t f_open(const uint8_t* filename);
int32_t d_open(const uint8_t* filename);

int32_t f_write(int32_t fd, const void* buf, int32_t bytes);
int32_t d_write(int32_t fd, const void *buf, int32_t bytes);

int32_t f_close(int32_t fd);
int32_t d_close(int32_t fd);

int32_t f_read(int32_t fd, void* buf, int32_t bytes);
int32_t d_read(int32_t fd, void* buf, int32_t bytes);

#endif //_FILESYSTEM_H
