//
// Created by hijoe on 4/30/2023.
//

#include "pcb.h"
#include "paging.h"
#include "definitions.h"
#include "filesystem.h"
#include "rtc.h"

volatile int active_terminal;
volatile int current_terminal;
volatile int next_terminal = -1;

volatile int terminal_pids[3];

volatile int terminal_x[3];
volatile int terminal_y[3];

volatile int halt_flag = -1;

void init_scheduler() {
    active_terminal = 0;
    current_terminal = -1;

    terminal_pids[0] = -1;
    terminal_pids[1] = -1;
    terminal_pids[2] = -1;

    return;
}

void switch_running_task(uint32_t pid) {
    pcb_t * curr = current_thread_PCB();

    register uint32_t curr_ebp asm("ebp");
    curr->ebp = curr_ebp;

    curr->ss0 = tss.ss0;
    curr->esp0 = tss.esp0;

    reload_tlb();

    int32_t idx = page_entry(VGA_TEXT_BUF_ADDR);

    if (current_terminal == active_terminal) {
        page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        page_table0[idx].base_addr = (VGA_TERM_0 + VGA_SIZE * current_terminal) >> TABLE_ADDRESS_SHIFT;
    }

    reload_tlb();

    pcb_t * next = PCB(pid);

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
    if (current_terminal >= 0 && active_terminal == index) return;
    int previous_terminal;

    previous_terminal = active_terminal;
    active_terminal = index;
    switch_video_mem(previous_terminal, active_terminal);

    int idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);

    if (current_terminal == active_terminal) {
        page_table1[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        page_table1[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    }
    if (terminal_pids[current_terminal] > 2) {
        idx = page_entry(VGA_TEXT_BUF_ADDR);
        if (active_terminal == current_terminal) {
            page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
        } else {
            page_table0[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
        }
    }
    reload_tlb();

    if (terminal_pids[active_terminal] == -1) {
        current_terminal = active_terminal;
        syscall_execute((uint8_t*) "shell");
    }
}

void switch_video_mem(uint32_t curr_term, uint32_t next_term) {
    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    idx = page_entry((VGA_TERM_0 + curr_term * VGA_SIZE));
    page_table0[idx].base_addr = (VGA_TERM_0 + curr_term * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    idx = page_entry((VGA_TERM_0 + next_term * VGA_SIZE));
    page_table0[idx].base_addr = (VGA_TERM_0 + next_term * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;

    reload_tlb();

    memcpy((void*) (VGA_TERM_0 + curr_term * VGA_SIZE), (void*) VGA_TEXT_BUF_ADDR, VGA_SIZE);
    memcpy((void*) VGA_TEXT_BUF_ADDR, (void*) (VGA_TERM_0 + next_term * VGA_SIZE), VGA_SIZE);

    reload_tlb();

    terminal_x[curr_term] = getX();
    terminal_y[curr_term] = getY();

    setX(terminal_x[next_term]);
    setY(terminal_y[next_term]);
}
