#include "schedule.h"
#define MULTI_THREAD 0

curr_term_sched = -1;
curr_term_displayed = 0;

#if (MULTI_THREAD == 0)
static int pid_to_sched_unit[3] = {-1, -1, -1};
#endif

void save_terminal(uint32_t terminal_idx){
    memcpy(VGA_TERM_0 + 0x1000*terminal_idx, VGA_TEXT_BUF_ADDR, 0x1000);
    terminal_arr[terminal_idx].cur_x = get_x();
    terminal_arr[terminal_idx].cur_y = get_y();
    memcpy(terminal_arr[terminal_idx].text_buf, kb_buf, 128);
    terminal_arr[terminal_idx].rtc_state = get_rtc_counter();
    terminal_arr[terminal_idx].rtc_target = get_rtc_target;
    terminal_arr[terminal_idx].curr_pcb = current;
}

void recover_terminal(uint32_t terminal_idx){
    memcpy(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + 0x1000*terminal_idx, 0x1000);
    set_x(terminal_arr[terminal_idx].cur_x);
    set_y(terminal_arr[terminal_idx].cur_y);
    memcpy(kb_buf, terminal_arr[terminal_idx].text_buf, 128);
    set_rtc_counter(terminal_arr[terminal_idx].rtc_state);
    set_rtc_target(terminal_arr[terminal_idx].rtc_target);

    // need to reset the pcb based on curr_pcb of terminal

}

void terminal_switch(uint32_t to){
    if(to == curr_term_displayed){
        return;
    }
    terminal* terminal = &terminal_arr[to];
    printf("SWITCHING TO TERMINAL #%d\n",to);

    switch(to){
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        default: break;
    }
}


void schedule(){
    switch(curr_term_sched){
        case -1:
            context_switch(0);
            break;
        case 0:
            context_switch(1);
            break;
        case 1:
            context_switch(2);
            break;
        case 2:
            context_switch(0);
            break;
        default: break;
    }
}


void context_switch(uint32_t to){
    // if terminal switching to is not running, execute shell
    if(process_using[to] == 0){
        syscall_execute("shell");
        return;
    }
    else{
        // if not save esp and ebp of current terminal, then recover the next terminal
        curr_term_sched = to;

        //update TSS to the stack we are going to
        // set TSS to parent's info
        tss.ss0 = KERNEL_DS;
        tss.esp0 = (1<<23) - ((1<<13)*(terminal_arr[to].curr_pid)) - 4; // Bottom of parent's kernel stack

        // map the user space based on terminal PID
        set_virtual_memory(terminal_arr[to].curr_pid);

        // remap video memory. We want to map it to actual video memory
        // if scheduled terminal and displayed terminal index is same
        if(curr_term_displayed == to){
            map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TEXT_BUF_ADDR);
        }
        else{
            map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + 0x1000 * to);
        }

        // save current ebp and esp
        asm volatile(
            "movl %%esp, %0                         \n\t"
            "movl %%ebp, %1                         \n\t"
            : "+r" (terminal_arr[curr_term_sched].esp_sched), "+r" (terminal_arr[curr_term_sched].ebp_sched)
            :
            : "cc"
        );
        asm volatile(
            "movl %0, %%esp                         \n\t"
            "movl %1, %%ebp                         \n\t"
            "leave                                  \n\t"
            "ret                                    \n\t"
            :
            : "r" (terminal_arr[to].esp_sched), "r" (terminal_arr[to].ebp_sched)
            : "cc"
        );
    }
}