#include "schedule.h"
#define MULTI_THREAD 0

#if (MULTI_THREAD == 0)
static int pid_to_sched_unit[3] = {-1, -1, -1};
#endif

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

static volatile int num_of_sched = 0;

static volatile int cur_sched = 0;
#if (MULTI_THREAD == 0)
int32_t context_switch(){
    // int32_t sched_index = sched_get_index(current);
    int32_t pid;
    if(num_of_sched<3){
        num_of_sched++;
        context_switch_to(-1,0);
    }else{
        pid = pid_to_sched_unit[cur_sched];
        cur_sched = (cur_sched + 1) % 3;
        if(pid==-1) return -1;
        // kprintf("pid %d\n", pid);
        context_switch_to(pid,0);
    }
    return 0;
}
#endif

int32_t context_switch_to(int32_t pid, int32_t tflag){
    pcb_t* cur_pcb = current;
    pcb_t* target_pcb = get_pcb_from_pos(pid);
    uint32_t flags;
    if(pid==-1){
        asm volatile(
            "movl %%esp, %0                         \n\t"
            "movl %%ebp, %1                         \n\t"
            "leal context_switch_return_addr, %2    \n\t"
            : "+r" (cur_pcb->esp), "+r" (cur_pcb->ebp), "+r" (cur_pcb->eip)
            :
            : "cc"
        );
        sti();
        syscall_execute((uint8_t*)"shell");
    }

    if(pid<0 || pid>=MAX_PROCESS_NUM) return -1;
    if(process_using[pid] != 1) return -1;

   
    //
    // Part 1 Set Paging
    //
    if(map_4MB_page(PROGRAM_START_VIRTUAL_ADDR, target_pcb->physical_mem_start)==-1) return -1;
    set_vidmap_present(pid,1);  //not sure!!
    
    //
    // Part 2 Set TSS
    //
    tss.ss0 = KERNEL_DS;
    tss.esp0 = get_kernel_stack_from_pos(target_pcb->pid);  //not sure!!!

    //
    // Part 3 Store Current Info
    //
    asm volatile(
        "movl %%esp, %0                         \n\t"
        "movl %%ebp, %1                         \n\t"
        "leal context_switch_return_addr, %2    \n\t"
        : "+r" (cur_pcb->esp), "+r" (cur_pcb->ebp), "+r" (cur_pcb->eip)
        :
        : "cc"
    );
#if (MULTI_THREAD == 0)
    if(tflag == 1){
        cli_and_save(flags);
        flags |= 0x0200;
        asm volatile(
            "pushl %0           \n\t"
            "pushl %1           \n\t"   // push user program esp
            "pushl %4           \n\t"
            "pushl %2           \n\t"   // push code segment information
            "pushl %3           \n\t"   // push user program eip
            "iret               \n\t"
            :
            : "r" ((uint32_t)USER_DS), "r" (target_pcb->esp), "r" ((uint32_t)USER_CS), "r" (target_pcb->eip), "r" (flags)
            : "eax"
        );
    }
#endif
    //
    // Part 4 Context Switch
    //
    asm volatile(
        "movl %0, %%esp                 \n\t"
        "movl %1, %%ebp                 \n\t"
        "movl %2, %%eax                 \n\t"
        "jmp *%%eax                     \n\t"
        "context_switch_return_addr:    \n\t"
        :
        : "r" (target_pcb->esp), "r" (target_pcb->ebp), "r" (target_pcb->eip)
        : "eax"
    );

    return 0;
}