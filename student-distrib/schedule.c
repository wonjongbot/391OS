#include "schedule.h"

void init_scheduler() {
    active_terminal_idx = 0;
    sched_terminal_idx = 0;
    active_process_idx = -1;
}

void schedule() {
    context_switch((sched_terminal_idx + 1) % terminal_count);
}

void terminal_switch(uint32_t idx) {
    save_terminal(active_terminal_idx);
    restore_terminal(idx);
    active_process_idx = terminals[active_terminal_idx].pid;

    active_terminal_idx = idx;
}

void context_switch(uint32_t idx) {
    register uint32_t ebp asm("ebp");
    terminals[sched_terminal_idx].ebp = ebp;

    int32_t pid = terminals[idx].pid;

    if (terminals[idx].pid == -1) {
        if (terminals[sched_terminal_idx].pid != -1) {
            save_terminal(sched_terminal_idx);
            restore_terminal(idx);
        }

        sched_terminal_idx = idx;
        active_terminal_idx = idx;
        syscall_execute((uint8_t*) "shell");
        return;
    }

    set_virtual_memory(pid);

    sched_terminal_idx = idx;

    tss.ss0 = KERNEL_DS;
    tss.esp0 = esp0(pid);

    asm volatile(
            "movl %0, %%ebp \n\t"
            "leave          \n\t"
            "ret            \n\t"
            :
            : "r" (terminals[sched_terminal_idx].ebp)
            : "ebp"
            );
}
