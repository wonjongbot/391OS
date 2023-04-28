#include "schedule.h"
#define MULTI_THREAD 0

// curr_term_sched = -1;
// curr_term_displayed = 0;


void save_terminal(uint32_t terminal_idx){
    //memcpy(VGA_TERM_0 + 0x1000*terminal_idx, VGA_TEXT_BUF_ADDR, 0x1000);
    terminal_arr[terminal_idx].cur_x = 0;//get_x();
    terminal_arr[terminal_idx].cur_y = 0;// get_y();
    terminal_arr[terminal_idx].rtc_state = get_rtc_counter();
    terminal_arr[terminal_idx].rtc_target = get_rtc_target();
    terminal_arr[terminal_idx].curr_pid = curr_pid;
    // terminal_arr[terminal_idx].curr_pcb = current;
}

void recover_terminal(uint32_t terminal_idx){
    //memcpy(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + 0x1000*terminal_idx, 0x1000);
    set_x(terminal_arr[terminal_idx].cur_x);
    set_y(terminal_arr[terminal_idx].cur_y);
    set_rtc_counter(terminal_arr[terminal_idx].rtc_state);
    set_rtc_target(terminal_arr[terminal_idx].rtc_target);
    curr_pid = terminal_arr[terminal_idx].curr_pid;

    // need to reset the pcb based on curr_pcb of terminal

}

void terminal_switch(uint32_t to){
    if(to == curr_term_displayed){
        return;
    }
    printf("SWITCHING TO TERMINAL #%d\n",to);

    // save current terminal content to curr_term_displayed
    memcpy((char*)(VGA_TERM_0 + 0x1000*curr_term_displayed), (char*)VGA_TEXT_BUF_ADDR, 0x1000);
    memcpy((char*)terminal_arr[curr_term_displayed].text_buf, kb_buf, 128);

    // recover terminal cntent from buffer
    memcpy((char*) VGA_TEXT_BUF_ADDR, (char*) (VGA_TERM_0 + 0x1000 * to), 0x1000);
    memcpy(kb_buf, terminal_arr[to].text_buf, 128);
    curr_term_displayed = to;
}


void schedule(){
    // printf("CURENT TERM: %d\n", curr_term_sched);
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
    int i;
    uint32_t prev_term_sched =curr_term_sched;
    curr_term_sched = to;

    // print current procs
    // printf("\n");
    // for(i = 0; i < 6; i++){
    //     printf("%d ", process_using[i]);
    // }
    // printf("\n");

    // printf("Current Terminal(to): %d\n", curr_term_sched);

    register uint32_t saved_ebp asm("ebp");

    terminal_arr[prev_term_sched].ebp_sched = saved_ebp;

    if(process_using[to] == 0){
        // printf("OPENING NEW SHELL at PID %d\n", to);

        syscall_execute((uint8_t*)"shell");
        return;
    }
    else{
        // printf("ALL TERMINALS INITIALIZED, JUMPING TO PID %d\n", terminal_arr[curr_term_sched].curr_pid);
        // if not save esp and ebp of current terminal, then recover the next terminal
        save_terminal(prev_term_sched);
        recover_terminal(curr_term_sched);

        //update TSS to the stack we are going to
        // set TSS to parent's info
        tss.ss0 = KERNEL_DS;
        tss.esp0 = (1<<23) - ((1<<13)*(curr_pid) - 4); // Bottom of parent's kernel stack

        // map the user space based on terminal PID
        set_virtual_memory(curr_pid);

        // remap video memory. We want to map it to actual video memory
        // if scheduled terminal and displayed terminal index is same
        if(curr_term_displayed == to){
            map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TEXT_BUF_ADDR);
        }
        else{
            map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + 0x1000 * curr_term_sched);
        }

        //uint32_t esp_tmp, ebp_tmp;

        // save current ebp and esp

        // while(1);

        asm volatile(
            "movl %0, %%ebp                         \n\t"
            "leave                                  \n\t"
            "ret                                    \n\t"
            :
            : "r" (terminal_arr[curr_term_sched].ebp_sched)
            : "cc"
        );
    }
}
