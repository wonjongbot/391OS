#include "multiterm.h"


void save_terminal(uint32_t terminal_idx){
    memcpy(VGA_TERM_0 + 0x1000*terminal_idx, VGA_TEXT_BUF_ADDR, 0x1000);
}

void recover_terminal(uint32_t terminal_idx){
    memcpy(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + 0x1000*terminal_idx, 0x1000);
}

void init_terminal(int terminal_idx){
    int32_t fd;
    pcb_t* curr;
    pcb_t* parent = NULL;
    if(curr_pid == -1){
        curr = current;
    }
    else{
        asm volatile(
        "addl %0, %%esp        \n\t"
        :
        : "r" (-8192 * terminal_idx)
        : "cc"
        );
        // initialize PCB
        curr = current;
    }
    PCB_init(curr);
    curr->parent = parent;

    // set virtual user space based on PID
    set_virtual_memory(curr->pid);

    fd = open((uint8_t*)"shell");

    printf("CURR PID: %d\n", curr->pid);
    
    filed file = curr->filearray[fd];
    uint32_t inode_idx = file.inode_index;
    uint32_t file_size = _inodes[inode_idx].length;
    read_data(inode_idx, 0, (uint8_t*)PROGRAM_START_VIRTUAL_ADDR, file_size);

    set_vidmap(curr->pid);
    curr_pid = curr->pid;
    // set TSS to child's information
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (1<<23) - ((1<<13)*(curr_pid)) - 4;  // Bottom of parent's kernel stack

    syscall_close(fd);
    register uint32_t esp_tmp asm("esp");
    printf("ESP VALUE in function: %x\n", esp_tmp);

    // initialize return address
    // terminal_arr[terminal_idx].saved_eip = *(uint32_t*) &(((uint8_t*) PROGRAM_START_VIRTUAL_ADDR)[24]);
}

void init_multiterm(){
    // initialize three 4MB pages for user program, and initialize pcb also
    
    // initialize three terminals
    // int i;
    // for(i = 0; i < 3; i++){
    //     init_terminal(i);
    //     // esp value returns to called function, PCB is at process 0 pretty neat
    //     register uint32_t esp_tmp asm("esp");
    //     printf("ESP VALUE after function call: %x\n", esp_tmp);
    // }
    // printf("CURR PID: %d\n", current->pid);
    // uint32_t return_addr = *(uint32_t*) &(((uint8_t*) PROGRAM_START_VIRTUAL_ADDR)[24]);
    // curr_term = 0;
    // printf("CURR TERMINAL: %d\n", curr_term);
    // asm volatile(
    //     "pushl %%eax \n\t"
    //     "pushl $0x83ffffc \n\t"
    //     "pushfl \n\t"
    //     "popl %%eax\n\t"    // set interrupt flag
    //     "orl $0x200, %%eax \n\t"
    //     "pushl %%eax \n\t"
    //     "pushl %%ebx \n\t"
    //     "pushl %%ecx \n\t"
    //     "iret \n\t"
    //     :
    //     : "a" (USER_DS), "b" (USER_CS), "c" (return_addr)
    //     : "cc", "memory"
    // );
    return;
}