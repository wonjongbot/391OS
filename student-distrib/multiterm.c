#include "multiterm.h"


void init_multiterm(){
    // initialize three 4MB pages for user program, and initialize pcb also
    int32_t fd;
    pcb_t* curr;
    pcb_t* parent = NULL;

    // initialize PCB
    curr = current;
    PCB_init(curr);
    curr->parent = parent;

    // set virtual user space based on PID
    set_virtual_memory(curr->pid);

    fd = open((uint8_t*)"shell");
    
    filed file = curr->filearray[fd];
    uint32_t inode_idx = file.inode_index;
    uint32_t file_size = _inodes[inode_idx].length;
    read_data(inode_idx, 0, (uint8_t*)PROGRAM_START_VIRTUAL_ADDR, file_size);

    set_vidmap(curr->pid);
    curr_pid = curr->pid;

    // set TSS to child's information
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (1<<23) - ((1<<13)*(curr_pid)) - 4;  // Bottom of parent's kernel stack
    uint32_t return_addr = *(uint32_t*) &(((uint8_t*) PROGRAM_START_VIRTUAL_ADDR)[24]);
    syscall_close(fd);
    printf("CURR PID: %d\n", curr->pid);
    asm volatile(
        "pushl %%eax \n\t"
        "pushl $0x83ffffc \n\t"
        "pushfl \n\t"
        "popl %%eax\n\t"    // set interrupt flag
        "orl $0x200, %%eax \n\t"
        "pushl %%eax \n\t"
        "pushl %%ebx \n\t"
        "pushl %%ecx \n\t"
        "iret \n\t"
        :
        : "a" (USER_DS), "b" (USER_CS), "c" (return_addr)
        : "cc", "memory"
    );
    // individually load shell program into those programs

    // do similar things that we did in execute. 

}