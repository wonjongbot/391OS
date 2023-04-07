

/*
- Set up some generic assembly linkage along lines of that used in Linux
- Including syscall value checking, register save and restore, and a jump table to C functions taht implement system calls themselves

    For this CP support system calls to run 
    - testprint from shell:execute, halt, 
    - open/close/read/write for teminal and fs
*/



#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H


#include "types.h"

uint32_t set_virtual_memory(uint32_t pcb_pos);

extern int32_t syscall_halt (uint8_t status);
extern int32_t syscall_execute (const uint8_t* command);

extern int32_t syscall_open (const uint8_t* filename);
extern int32_t syscall_close (int32_t fd);
extern int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t syscall_write (int32_t fd, const void* buf, int32_t nbytes);

// User Level Wrapper
extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);



#endif
