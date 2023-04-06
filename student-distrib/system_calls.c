

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


/* 
 * open
 * Input: filename
 * Output: -1 If the named file does not exist or no descriptors are free, 0 on Success
 * The open system call provides access to the file system. The call should find the directory entry corresponding to the
 * named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,RTC device, or regular file)
 */
int32_t open (const uint8_t* filename) {
    // If the named file does not exist or no descriptors are free, the call returns -1
    
    // find the directory entry corresponding to the named file
    // Allocate unused file descriptor
    // Set up any data necessary to handle the given type of file (directory, RTC device, or regular file)
}

int32_t close (int32_t fd) {
/*
The close system call closes the specified file descriptor and makes it available for return from later calls to open.
You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
descriptor should result in a return value of -1; successful closes should return 0.
*/
}

int32_t read (int32_t fd, void* buf, int32_t nbytes) {
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

int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
/*
The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
written, or -1 on failure.
*/
}
