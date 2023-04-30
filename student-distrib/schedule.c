//
// Created by hijoe on 4/30/2023.
//

#include "pcb.h"
#include "paging.h"
#include "definitions.h"
#include "filesystem.h"

volatile int active_terminal;
volatile int current_terminal;
volatile int next_terminal = -1;

volatile int terminal_pids[3];

void init_scheduler() {
    active_terminal = 0;
    current_terminal = -1;

    terminal_pids[0] = -1;
    terminal_pids[1] = -1;
    terminal_pids[2] = -1;

    return;
}

void switch_running_task(uint32_t pid) {
    pcb_t* curr = current_thread_PCB();

    register uint32_t curr_ebp asm("ebp");
    curr->ebp = curr_ebp;

    curr->ss0 = tss.ss0;
    curr->esp0 = tss.esp0;

    reload_tlb();

    PTE temp;
    int32_t idx = dir_entry(VGA_TEXT_BUF_ADDR);
    temp.val = page_table0[idx].val;

    if (current_terminal == active_terminal) {
        temp.base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        temp.base_addr = (VGA_TERM_0 + VGA_SIZE * current_terminal) >> TABLE_ADDRESS_SHIFT;
    }

    page_table0[idx].val = temp.val;

    reload_tlb();

    pcb_t* next = PCB(pid);

    tss.ss0 = next->ss0;
    tss.esp0 = next->esp0;

    asm volatile("movl %0, %%ebp  \n\t"
                 :
                 : "g"(next->ebp)
                 );

    asm volatile("leave           \n\t"
                 "ret             \n\t");

    return;
}

void switch_active_terminal(uint32_t index) {
    int previous_terminal;

    PTE temp;

    current_terminal++;
    if (current_terminal > 2) current_terminal = 0;

    previous_terminal = active_terminal;
    active_terminal = index;
    switch_video_mem(previous_terminal, active_terminal);

    int idx = dir_entry(VIDMAP_START_VIRTUAL_ADDR);
    temp.val = page_table1[idx].val;

    if (current_terminal == active_terminal) {
        temp.base_addr = VGA_TEXT_BUF_ADDR >> 12;
    } else {
        temp.base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> 12;
    }

    page_table1[idx].val = temp.val;

    if (terminal_pids[current_terminal] == -1) {
        syscall_execute((uint8_t*) "shell");
    }
}

void switch_video_mem(uint32_t curr_term, uint32_t next_term) {
    PTE temp;
    uint32_t idx = dir_entry(VGA_TEXT_BUF_ADDR);

    temp.base_addr = VGA_TEXT_BUF_ADDR;
    page_table0[idx].val = temp.val;

    reload_tlb();

    memcpy((void*) (VGA_TERM_0 + curr_term * VGA_SIZE), (void*) VGA_TEXT_BUF_ADDR, VGA_SIZE);
    memcpy((void*) VGA_TEXT_BUF_ADDR, (void*) (VGA_TERM_0 + next_term * VGA_SIZE), VGA_SIZE);

    reload_tlb();
}

void start_shell(uint32_t pid) {
    printf("help\n");
    dentry_t shell;
    uint8_t buf[4];

    read_dentry_by_name((uint8_t*) "shell", &shell);
    read_data(shell.inode, 24, buf, 4);

    uint32_t entry = *(uint32_t*) buf;


    printf("%d\n", pid);
    set_virtual_memory(pid);

    read_data(shell.inode, 0, (uint8_t*)(PROGRAM_START_VIRTUAL_ADDR), _inodes[shell.inode].length);

    PCB_init(PCB(pid));


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
                : "a" (USER_DS), "b" (USER_CS), "c" (entry)
                : "cc", "memory"
                );
}
