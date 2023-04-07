

/*
- Set up some generic assembly linkage along lines of that used in Linux
- Including syscall value checking, register save and restore, and a jump table to C functions taht implement system calls themselves

    For this CP support system calls to run 
    - testprint from shell:execute, halt, 
    - open/close/read/write for teminal and fs
*/

#include "system_calls.h"
#include "lib.h"
#include "types.h"
#include "filesystem.h"
#include "rtc.h"
#include "pcb.h"
#include "paging.h"



static file_ops rtc_ops_list = {rtc_read, rtc_write, rtc_open, rtc_close};
static file_ops dir_ops_list = {d_read, d_write, d_open, d_close};
static file_ops file_ops_list = {f_read, f_write, f_open, f_close};

/*  
 * sys_halt
 *   DESCRIPTION: set the vitual memory by page for pcb
 *   INPUTS: pcb position
 *   OUTPUTS: 0
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
uint32_t set_virtual_memory(uint32_t pcb_pos){
    if(pcb_pos>=MAX_PROCESS_NUM) return -1;
    uint32_t pde_index = dir_entry(PROGRAM_START_VIRTUAL_ADDR);
    page_directory[pde_index].val = 0;
    page_directory[pde_index].present = 1;
    page_directory[pde_index].ps = 1;
    page_directory[pde_index].rw = 1;
    page_directory[pde_index].us = 1;
    page_directory[pde_index].val |= ((uint32_t)((pcb_pos+2) * PAGE_4MB_VAL));
    
    reload_tlb();
    return 0;
}
/*  
 * sys_halt
 *   DESCRIPTION: syetem call: terminate a process
 *   INPUTS: status :  return value to the parent 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t syscall_halt (uint8_t status){
    return 0;
}

/*  
 * sys_execute
 *   DESCRIPTION: system call: load, set and execute a new program
 *   INPUTS: const uint8_t* command -- the command to be executed
 *   OUTPUTS: none
 *   RETURN VALUE: 256 if occur exception
 *                 0 to 255 if execute successfully
 *                 -1 if failed
 *   SIDE EFFECTS: none
 */
int32_t syscall_execute(const uint8_t* command){
    return 0;
}

/* 
 * open
 * Input: filename
 * Output: -1 If the named file does not exist or no descriptors are free, 0 on Success
 * The open system call provides access to the file system. The call should find the directory entry corresponding to the
 * named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,RTC device, or regular file)
 */
int32_t syscall_open (const uint8_t* filename) {
    if(filename==NULL) return -1;
    int32_t fd;
    dentry_t dentry;

    //find what we need to start file's fd
    for(fd=2; ;fd++){
        if(current->filearray[fd].flags == 0){
            break;
        }
        if (fd==FILEARR_SIZE){
            return -1;
        }
    }

    if(read_dentry_by_name(filename, &dentry) == -1) return -1;

    filed* filenew = &current->filearray[fd];
    filenew->flags = 1;
    filenew->file_position = 0;
    filenew->inode_index = dentry.inode; 

    if (dentry.file_type==0){
        filenew->ops = &rtc_ops_list;    // 0 for RTC
    }
    else if (dentry.file_type==1){
        filenew->ops = &dir_ops_list;    // 1 for dir
    }
    else if (dentry.file_type==2){
        filenew->ops = &file_ops_list;    // 2 for file
    }
    else{ return -1;}

    if(filenew->ops->open(filename) == -1) return -1;

    //not sure if we return fd or 0
    return fd;

    // If the named file does not exist or no descriptors are free, the call returns -1
    
    // find the directory entry corresponding to the named file
    // Allocate unused file descriptor
    // Set up any data necessary to handle the given type of file (directory, RTC device, or regular file)
}

int32_t syscall_close (int32_t fd) {
    if(fd>=8 || fd<0) return -1;    //0=<fd<8

    filed* fileclose = &current->filearray[fd];
    if(fileclose->flags == 0 || fileclose->ops==NULL) return -1;

    uint32_t close_value=fileclose->ops->close(fd);

    //free
    fileclose->flags = 0;
    fileclose->inode_index = -1;
    fileclose->ops = NULL;
    fileclose->file_position = 0;

    //not sure what to return
    return close_value;

/*
The close system call closes the specified file descriptor and makes it available for return from later calls to open.
You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
descriptor should result in a return value of -1; successful closes should return 0.
*/
}

int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes) {
    if(fd>=8 || fd<0) return -1;    //0=<fd<8
    if(current->filearray[fd].flags == 0) return -1;
    return current->filearray[fd].ops->read(fd, buf, nbytes);
/*
The read system call reads data from the keyboard, a file, device (RTC), or directory. This call returns the number
of bytes read. If the initial file position is at or beyond the end of file, 0 shall be returned (for normal files and the
directory). In the case of the keyboard, read should return data from one line that has been terminated by pressing
Enter, or as much as fits in the buffer from one such line. The line returned should include the line feed character.
In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
sooner. In the case of reads to the directory, only the filename should be provided (as much as fits, or all 32 bytes), and
subsequent reads should read from successive directory entries until the last is reached, at which point read should
repeatedly return 0. For the real-time clock (RTC), this call should always return 0, but only after an interrupt has
occurred (set a flag and wait until the interrupt handler clears it, then return 0). You should use a jump table referenced
by the task’s file array to call from a generic handler for this call into a file-type-specific function. This jump table
should be inserted into the file array on the open system call (see below).
*/
}

int32_t syscall_write (int32_t fd, const void* buf, int32_t nbytes) {
    if(fd>=8 || fd<0) return -1;    //0=<fd<8
    if(current->filearray[fd].flags == 0) return -1;
    return current->filearray[fd].ops->write(fd, buf, nbytes);
/*
The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
written, or -1 on failure.
*/
}
