

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

extern int32_t sys_halt (uint8_t status);\
extern int32_t sys_execute (const uint8_t* command);

extern int32_t sys_open (const uint8_t* filename);
extern int32_t sys_close (int32_t fd);
extern int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);



#endif
