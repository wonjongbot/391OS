#include "schedule.h"


void save_curr_terminal(){
    memcpy(VGA_TERM_0 + 0x1000*curr_term, VGA_TEXT_BUF_ADDR, 0x1000);
    terminal_arr[curr_term].cur_x = get_x();
    terminal_arr[curr_term].cur_y = get_y();
    memcpy(terminal_arr[curr_term].text_buf, kb_buf, 128);
    terminal_arr[curr_term].rtc_state = get_rtc_counter();
    terminal_arr[curr_term].rtc_target = get_rtc_target;
    terminal_arr[curr_term].curr_pcb = current;
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
// the bottom two functions switch_terminal_graphics and switch_terminal are WIP. Nothing is done in terms of 
// swithcing to different terminal. I was testing out different stuff.
void switch_terminal_graphics(uint32_t to){
    if(to < 0 || to > 2 || to == curr_term){
        return;
    }
    save_curr_terminal();

    switch(to){
        case 0:
            switch_terminal_graphics(0, 1);
            break;
        case 1:
            switch_terminal_graphics(1, 2);
            break;
        case 2:
            switch_terminal_graphics(2, 0);
            break;
        default: break;
    }
    recover_terminal(to);
}

void switch_terminal(uint32_t term_idx_src){
    if(term_idx_src == curr_term){
        return;
    }
    terminal* terminal = &terminal_arr[term_idx_src];
    printf("SWITCHING TO TERMINAL #%d\n",term_idx_src);
    printf("CURR PID: %d\n", current->pid);
    printf("CURR TERMINAL: %d\n", curr_term);

    switch(term_idx_src){
        case 0:
            switch_terminal_graphics(0, 1);
            break;
        case 1:
            switch_terminal_graphics(1, 2);
            break;
        case 2:
            switch_terminal_graphics(2, 0);
            break;
        default: break;
    }


    curr_term = (term_idx_src + 1) % 2;
    //need to set pcb and vga
    //didn't finish!!
         
}

void switch_context(uint32_t term_idx_src){

}


void schedule(){

}