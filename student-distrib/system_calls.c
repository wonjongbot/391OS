

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
#include "terminal.h"
#include "schedule.h"
#include "sound.h"

// current executing EXE's fd # in parent process.
// int32_t opened_exe_fd = 0;

static file_ops_t rtc_ops_list = {rtc_read, rtc_write, rtc_open, rtc_close};
static file_ops_t dir_ops_list = {d_read, d_write, d_open, d_close};
static file_ops_t file_ops_list = {f_read, f_write, f_open, f_close};
//static file_ops_t sound_ops_list = {sound_read, sound_write, sound_open, sound_close};
/*
 * sys_halt
 *   DESCRIPTION: set the vitual memory by page for pcb
 *   INPUTS: pcb position
 *   OUTPUTS: None
 *   RETURN VALUE: base address of new page
 *   SIDE EFFECTS: none
 */
void set_virtual_memory(uint32_t pcb_id) {
    if (pcb_id >= MAX_PROCESS_NUM) return;
    uint32_t pde_index = dir_entry(PROGRAM_START_VIRTUAL_ADDR);
    page_directory[pde_index].val = 0;
    page_directory[pde_index].present = 1;
    page_directory[pde_index].ps = 1;
    page_directory[pde_index].rw = 1;
    page_directory[pde_index].us = 1;
    page_directory[pde_index].base_addr = ((uint32_t) ((pcb_id + 2) * PAGE_4MB_VAL)) >> 12;

    reload_tlb();
}

void set_vidmap_present(uint32_t pcb_id, uint32_t present) {
    if (pcb_id >= MAX_PROCESS_NUM) return;

    uint32_t pde_index = dir_entry(VIDMAP_START_VIRTUAL_ADDR);
    page_directory[pde_index].val = 0;
    page_directory[pde_index].present = 1;
    page_directory[pde_index].ps = 0;
    page_directory[pde_index].rw = 1;
    page_directory[pde_index].us = 1;
    page_directory[pde_index].base_addr = ((uint32_t) page_table1 & ALIGNED_ADDR_MASK) >> TABLE_ADDRESS_SHIFT;

    uint32_t pte_index = page_entry(VIDMAP_START_VIRTUAL_ADDR);
    page_table1[pte_index].val = 0;
    page_table1[pte_index].present = present;
    page_table1[pte_index].ps = 0;
    page_table1[pte_index].rw = 1;
    page_table1[pte_index].us = 1;
    page_table1[pte_index].base_addr =
            ((current_terminal == active_terminal ? VGA_TEXT_BUF_ADDR : (VGA_TERM_0 + current_terminal * VGA_SIZE)) &
             ALIGNED_ADDR_MASK) >> TABLE_ADDRESS_SHIFT;

    reload_tlb();
}

#define abnormal_ret 0xff
int32_t halt_ret;

/*
 * sys_halt
 *   DESCRIPTION: syetem call: terminate a process
 *   INPUTS: status :  return value to the parent
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t syscall_halt(uint8_t status) {
    // How do we know if this is an exception call?
    //pcb_t * curr = current;
    pcb_t * curr = PCB(terminal_pids[current_terminal]);
//    printf("CALLING HALT ON PID %d\n", terminal_pids[curr->terminal_idx]);
    halt_ret = (int32_t) status;
    int i;
    // close all the file descriptor
    for (i = 2; i < 8; i++) {
        syscall_close(i);
    }

    // -0x1f is the smallest exception number in uint8_t scale
    if (halt_ret > (uint8_t) -0x1f && halt_ret != abnormal_ret) {
        set_attrib(0x4E);
        printf("[!] User program terminated by exception");
        set_attrib(0x7);
        printf("\n");
        halt_ret = 256;
    }

    set_vidmap_present(curr->pid, 0);

    if (curr->parent == NULL) {
        set_attrib(0x4E);
        printf("[!] Base shell cannot be closed! Restarting shell...");
        set_attrib(0x7);
        printf("\n");
        curr->status = 0;
        pid_dealloc(terminal_pids[curr->terminal_idx]);
        terminal_pids[current_terminal] = -1;
        syscall_execute((uint8_t*) "shell");
    } else {
        // set current pid as inactive
        curr->status = 0;
        pid_dealloc(terminal_pids[curr->terminal_idx]);
        curr->terminal_idx = -1;


        // update tss to saved of parent
        pcb_t * parent = curr->parent;

        parent->ss0 = tss.ss0;
        parent->esp0 = tss.esp0;

//        printf("HALTING: PARENT PID IS %d\n", parent->pid);

        terminal_pids[parent->terminal_idx] = parent->pid;

        // unmap current paging / map parent's paging using physical_mem_start
        set_virtual_memory(parent->pid);

        set_vidmap_present(parent->pid, 0);


        tss.ss0 = KERNEL_DS;

        //NOT SURE WHAT THE VALUE HERE SHOULD BE
        // tss.esp0 = parent->save_esp;
        tss.esp0 = (1 << 23) - ((1 << 13) * (parent->pid)) - 4;



        //tss.esp0 = curr->save_esp;
        // printf("PID: %d\n", curr_pid);
        // set esp and ebp to parent's saved esp and
        asm volatile(
                "movl %0, %%esp        \n\t"
                "movl %1, %%ebp        \n\t"
                "movl %2, %%eax        \n\t"
                :
                : "r"(parent->save_esp), "r"(parent->save_ebp), "r"(halt_ret)
                : "cc"
                );

    }
    // syscall_close(opened_exe_fd);
    // printf("STATUS: %d\n", halt_ret);
    return halt_ret;
}

#define MAGIC_NUM_SIZE 4
#define MAGIC_NUM_EXE 0x464c457f

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
int32_t syscall_execute(const uint8_t* command) {
    if (command == NULL) return -1;

// // if there is no process (first shell), we want to set up PCB in current stack
    if (pid_peek() == -1) {
        set_attrib(0x4E);
        printf("[!] Maximum number of process opened! Close programs to allocate more...");
        set_attrib(0x7);
        printf("\n");
        return -1;
    }

    // open it in the current process before spawning child process // edit, no need to do this anymore
    // since peizhe told us that executables shouldn't be in fd array
//  if ((fd = open(command)) == -1){
//    return -1;
//  }
//  opened_exe_fd = fd;
// //  syscall_close(fd);

    pcb_t * parent = NULL;
    pcb_t * curr;
    if (terminal_pids[current_terminal] == -1) {
        printf("Welcome to Terminal %d\n", current_terminal);
//        if (current_terminal > 0) {
//            asm volatile(
//                    "addl $-8192, %%esp        \n\t"
//                    :
//                    :
//                    : "cc"
//                    );
//        }
        curr = PCB(pid_peek());
    } else {
        // save ebp and esp
        parent = PCB(terminal_pids[current_terminal]);
//        printf("CALLING EXECUTE WITH PARENT with PID %d\n", parent->pid);

        register uint32_t ebp_tmp asm("ebp");
        parent->save_ebp = ebp_tmp;

        register uint32_t esp_tmp asm("esp");
        parent->save_esp = esp_tmp;
//        asm volatile(
//                "addl $-8192, %%esp        \n\t"
//                :
//                :
//                : "cc"
//                );
        curr = PCB(pid_peek());
    }

    if (PCB_init(curr) == -1) return -1;

    curr->parent = parent;

    uint32_t command_size = strlen((int8_t*) command);
    uint32_t name_size = 0;
    while (name_size < command_size && command[++name_size] != ' ');

    uint8_t program[name_size + 1];
    memcpy(program, command, name_size);
    program[name_size] = '\0';

    memset(curr->argv, '\0', ARGV_MAX_LEN + 1);

    if (name_size == strlen("rtc") && strncmp((int8_t*) program, "rtc", strlen("rtc")) == 0) {
        unload(curr, -1);
        return -1;
    }

    if (name_size == strlen("proc") && strncmp((int8_t*) program, "proc", strlen("proc")) == 0) {
        print_proc();
        unload(curr, 0);
        return 0;
    }

    // Remove any extra whitespace after program name
    if (command_size > name_size) {
        while (name_size < command_size && command[++name_size] == ' ');
        memcpy(curr->argv, &command[name_size], command_size - name_size);
    }

    dentry_t dentry;
    if (read_dentry_by_name(program, &dentry) == -1) {
        unload(curr, -1);
        return -1;
    }

    uint32_t inode_idx = dentry.inode;
    uint32_t file_size = _inodes[inode_idx].length;


    // find out entry point is correct entry point is valid

    // also have to do TSS stuff

    uint8_t buf[MAGIC_NUM_SIZE];

    if ((read_data(inode_idx, 0, buf, MAGIC_NUM_SIZE) == -1) || *(uint32_t*) buf != MAGIC_NUM_EXE) {
        unload(curr, -1);
        return -1;
    }
    set_virtual_memory(curr->pid);

    //copy to user space
    if (read_data(inode_idx, 0, (uint8_t*) PROGRAM_START_VIRTUAL_ADDR, file_size) == -1) {
        unload(curr, -1);
        return -1;
    }

    set_vidmap_present(curr->pid, 0);

    int tmp = 0;
    while(tmp <= name_size){
        *(curr->prog_name + tmp) = *(program + tmp);
        tmp++;
    }


    // remap display addr?
    int idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);
    page_table1[idx].base_addr = VGA_TEXT_BUF_ADDR >> 12;

    curr->terminal_idx = current_terminal;
    terminal_pids[curr->terminal_idx] = curr->pid;
//    printf("adding terminal %d pid: %d\n", curr->terminal_idx, terminal_pids[curr->terminal_idx]);

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (1 << 23) - ((1 << 13) * (curr->pid)) - 4;

    curr->esp0 = tss.esp0;
    curr->ss0 = tss.ss0;

    uint32_t return_addr = *(uint32_t*) &(((uint8_t*) PROGRAM_START_VIRTUAL_ADDR)[24]);

    asm volatile(
            "pushl %%eax \n\t"
            "pushl $0x83ffffc \n\t"
            "pushfl \n\t"
            "popl %%eax\n\t"    // set interrupt flag
            "orl $0x200, %%eax \n\t"
            "pushl %%eax \n\t"
            "pushl %%ebx \n\t"
            "pushl %%ecx \n\t"
            :
            : "a" (USER_DS), "b" (USER_CS), "c" (return_addr)
            : "cc", "memory"
            );

    asm volatile(
            "iret \n\t"
            :
            :
            : "memory"
            );
    return 0;
}

/*
 * open
 * Input: filename
 * Output: -1 If the named file does not exist or no descriptors are free, 0 on Success
 * The open system call provides access to the file system. The call should find the directory entry corresponding to the
 * named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,RTC device, or regular file)
 */
int32_t syscall_open(const uint8_t* filename) {
    // int i;
    // for(i = 0; i < FILEARR_SIZE; i++){
    //   printf("%d ", current->filearray[i].flags);
    // }
    // printf("\n");
    if (filename == NULL) return -1;
    int32_t fd;
    dentry_t dentry;

    //find what we need to start file's fd
    for (fd = 2;; fd++) {
        if (fd == FILEARR_SIZE) {
            return -1;
        }
        if (current->filearray[fd].flags == 0) {
            break;
        }
    }

    if (read_dentry_by_name(filename, &dentry) == -1) return -1;

    filed* filenew = &current->filearray[fd];
    filenew->flags = 1;
    filenew->file_position = 0;
    filenew->inode_index = dentry.inode;

    if (dentry.file_type == 0) {
        filenew->ops = &rtc_ops_list;    // 0 for RTC
    } else if (dentry.file_type == 1) {
        filenew->ops = &dir_ops_list;    // 1 for dir
    } else if (dentry.file_type == 2) {
        filenew->ops = &file_ops_list;    // 2 for file
    } else { return -1; }

    if (filenew->ops->open(filename) == -1) return -1;
    //if (strncmp((int8_t*)filename, (int8_t*)"sound",strlen((int8_t*)filename))){
        //filenew->ops = &sound_ops_list;}
    //not sure if we return fd or 0
    return fd;

    // If the named file does not exist or no descriptors are free, the call returns -1

    // find the directory entry corresponding to the named file
    // Allocate unused file descriptor
    // Set up any data necessary to handle the given type of file (directory, RTC device, or regular file)
}

int32_t syscall_close(int32_t fd) {
    if (fd >= 8 || fd < 2) return -1;    //2=<fd<8

    filed* fileclose = &current->filearray[fd];
    if (fileclose->flags == 0 || fileclose->ops == NULL) return -1;

    uint32_t close_value = fileclose->ops->close(fd);

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

int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes) {
    if (fd >= 8 || fd < 0) return -1;    //0=<fd<8
    if (PCB(terminal_pids[current_terminal])->filearray[fd].flags == 0) return -1;
    sti();
    return PCB(terminal_pids[current_terminal])->filearray[fd].ops->read(fd, buf, nbytes);
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

int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
// printf("SYSCALL WRITE on fd %d\n", fd);
//  printf("CURRENT PCB ADDR: %x\n", current);
    if (fd >= 8 || fd < 0) return -1;    //0=<fd<8
////    printf("current terminal PID writing: %d\n", terminal_pids[current_terminal]);
//    printf("What does this think %d\n", terminal_pids[current->terminal_idx]);
//
//    printf("This %x and This %x\n", PCB(terminal_pids[current->terminal_idx]), current);

    if (PCB(terminal_pids[current->terminal_idx])->filearray[fd].flags == 0) return -1;
    return PCB(terminal_pids[current->terminal_idx])->filearray[fd].ops->write(fd, buf, nbytes);
/*
The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
written, or -1 on failure.
*/
}

/*
 * syscall_getargs
 * DESCRIPTION: system call: returns the argument saved before
 *
 *  Inputs: buffer-- stores the argument
 *          nbytes: number of bytes in the buffer
 *
 *  Returns: -1: if failed
 *            0: if success
 *
 *  Side effects: none
 *
 */
int32_t syscall_getargs(uint8_t* buf, int32_t nbytes) {
    // check if pointer is in user space
    if ((uint32_t) buf < VALUE_128MB || (uint32_t) buf >= VALUE_132MB) return -1;
    pcb_t * curr = current;
    int32_t n = strlen((int8_t*) curr->argv);
    if (n + 1 > nbytes || n == 0) return -1;

    memcpy(buf, curr->argv, n + 1);
    return 0;
}

/*
 * syscall_vidmap
 *   DESCRIPTION: system call: map the text-mode video memory into user-space virtual address
 *   INPUTS: screen_start
 *   OUTPUTS: none
 *  Returns: -1: if failed
 *            0: if success
 *   SIDE EFFECTS: none
 */
int32_t syscall_vidmap(uint8_t** screen_start) {
    // check if pointer is in user space
    if ((uint32_t) screen_start < VALUE_128MB || (uint32_t) screen_start >= VALUE_132MB) return -1;

    uint32_t pte_index = page_entry(VIDMAP_START_VIRTUAL_ADDR);
    page_table1[pte_index].present = 1;

    //cli_and_save(flags);
    *screen_start = (uint8_t*) VIDMAP_START_VIRTUAL_ADDR;
    //restore_flags(flags);

    return 0;
}

// Unimplemented stub
// TODO
int32_t syscall_set_handler(int32_t signum, void* handler_address) {
    return -1;
}

// Unimplemented stub
// TODO
int32_t syscall_sigreturn(void) {
    return -1;
}
