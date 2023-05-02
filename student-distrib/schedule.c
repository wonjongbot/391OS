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

volatile int terminal_vidmap_present[3];

volatile int halt_flag = -1;


/*
 * init_scheduler
 * Input: None
 * Output: None
 * Return value: none
 * initializes all the necessary variables for scheduling/multiple terminal support
 */
void init_scheduler() {
    active_terminal = 0;
    current_terminal = -1;
    current->terminal_idx = current_terminal;

    int i;
    for(i = 0; i < 6; i++){
        PCB(i)->terminal_idx = -1;
    }

    terminal_pids[0] = -1;
    terminal_pids[1] = -1;
    terminal_pids[2] = -1;

    return;
}

int32_t terminal_ebp[3];

/*
 * switch_running_task
 * Input: pid -- current running process ID for current terminal
 * Output: None
 * Return value: None
 * Task switching function for scheduling. Our attempt jumps from one stack frame to another and does C callee teardown.
 * This function is not used because we were not able to finish scheduling. so things are left uncommented
 */
void switch_running_task(uint32_t terminal_to) {

    int previous_terminal;

    previous_terminal = current_terminal;
    current_terminal = terminal_to;


    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;


    if(current_terminal == active_terminal){
        idx = page_entry(VGA_TEXT_BUF_ADDR);
        page_table0[idx].base_addr = (VGA_TEXT_BUF_ADDR) >> TABLE_ADDRESS_SHIFT;
    }
    else{
        idx = page_entry(VGA_TEXT_BUF_ADDR);
        page_table0[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    }
    reload_tlb();
    // for vidmap
    idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);
    // if current terminal on schedular is same as displayed terminal, we draw on the actual text buffer addr
    if (current_terminal == active_terminal) {
        page_table1[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        // if not then we want to draw on the undisplayed buffer one.
        page_table1[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    }
    reload_tlb();

    if(previous_terminal != -1) {
        pcb_t * curr = PCB(terminal_pids[previous_terminal]);

        curr->ss0 = tss.ss0;
        curr->esp0 = tss.esp0;

        idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);
        terminal_vidmap_present[previous_terminal] = page_table1[idx].present;
    }


    if(previous_terminal >= 0){
//        printf("CURRENTLY IN TERMINAL %d\n", current->terminal_idx);
//        printf("PREVIOUS TERMINAL INDEX IS %d\n", previous_terminal);
        register uint32_t curr_ebp asm("ebp");
        terminal_ebp[previous_terminal] = curr_ebp;
    }
    if(terminal_pids[current_terminal] == -1){
        clear();
        reset_text_cursor();
        printf("CURRENT TERMINAL %d\n", current_terminal);
        printf("No active proc in curent terminal. Executing shell\n");
        syscall_execute((uint8_t*)"shell");
    } else{

        pcb_t * next = PCB(terminal_pids[current_terminal]);

        tss.ss0 = next->ss0;
        tss.esp0 = next->esp0;

        // save the cursor location of current terminal
        if(previous_terminal != -1) {
            terminal_x[previous_terminal] = getX();
            terminal_y[previous_terminal] = getY();
        }

        // recover where the cursor was in the terminal we are swithcing to.
        setX(terminal_x[current_terminal]);
        setY(terminal_y[current_terminal]);

        idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);
        page_table1[idx].present = terminal_vidmap_present[current_terminal];

        set_virtual_memory(next->pid);
        cursor_to_coord(terminal_x[active_terminal], terminal_y[active_terminal]);
        reload_tlb();
//        printf("swithing to %x\n",terminal_ebp[current_terminal]);
        asm volatile("movl %0, %%ebp  \n\t"
                :
                : "g"(terminal_ebp[current_terminal])
                );

        asm volatile("leave           \n\t"
                     "ret             \n\t");
    }
}

/*
 * switch_active_terminal
 * Input: pid -- current running process ID for current terminal
 * Output: None
 * Return value: None
 * Function for switching active (visible) terminal on keyboard interrupt (Fn + # key).
 * Shell is executed if the terminal switches to an uninitialized terminal
 */
void switch_active_terminal(uint32_t index) {
    if(index == active_terminal) return;
    // if we are switching to the currently displayed terinal, no work is done
//    if (current_terminal >= 0 && active_terminal == index) return;
    int previous_terminal;

    // update our global variable trackers for currently active (displayed) terminal index and previous terminal index
    previous_terminal = active_terminal;
    active_terminal = index;

//    terminal_x[current_terminal] = getX();
//    terminal_y[current_terminal] = getY();

//    terminal_x[previous_terminal] = getX();
//    terminal_y[previous_terminal] = getY();

    switch_video_mem(previous_terminal, active_terminal);

//    setX(terminal_x[active_terminal]);
//    setY(terminal_y[active_terminal]);
    cursor_to_coord(terminal_x[active_terminal], terminal_y[active_terminal]);

    // map the page so that previous active page draws to the buffered video memory, not the actual displayed video
    //      memory. This part is for scheduling.
    int idx = page_entry(VIDMAP_START_VIRTUAL_ADDR);
    // if current terminal on schedular is same as displayed terminal, we draw on the actual text buffer addr
    if (current_terminal == active_terminal) {
        page_table1[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        // if not then we want to draw on the undisplayed buffer one.
        page_table1[idx].base_addr = (VGA_TERM_0 + index * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    }
    reload_tlb();
}


/*
 * switch_video_mem
 * Input: curr_term - source terminal index
 *        next_term - destination terminal index
 * Output: None
 * Return value: None
 * helper function that maps/unmaps VGA pages, saves and recovers screen data for terminal switching
 */
void switch_video_mem(uint32_t curr_term, uint32_t next_term) {

    // map virtual addresses so that all VGA pages (including invisible buffer ones)
    //      have their physical and virtual address to be 1:1 mapped
    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    idx = page_entry((VGA_TERM_0 + curr_term * VGA_SIZE));
    page_table0[idx].base_addr = (VGA_TERM_0 + curr_term * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    idx = page_entry((VGA_TERM_0 + next_term * VGA_SIZE));
    page_table0[idx].base_addr = (VGA_TERM_0 + next_term * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;

//    if(current_terminal != active_terminal){
//        uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
//        page_table0[idx].base_addr = (VGA_TERM_0 + next_term * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
//    }

    reload_tlb();

    // Save our current screen to the buffered screen page dedicated for current terminal index
    memcpy((void*) (VGA_TERM_0 + curr_term * VGA_SIZE), (void*) VGA_TEXT_BUF_ADDR, VGA_SIZE);
    // then recover video memory of next terminal we are swithcing to
    memcpy((void*) VGA_TEXT_BUF_ADDR, (void*) (VGA_TERM_0 + next_term * VGA_SIZE), VGA_SIZE);


    reload_tlb();

//    // save the cursor location of current terminal
//    terminal_x[curr_term] = getX();
//    terminal_y[curr_term] = getY();
//
//    // recover where the cursor was in the terminal we are swithcing to.
//    setX(terminal_x[next_term]);
//    setY(terminal_y[next_term]);

//    cursor_to_coord(terminal_x[next_term], terminal_y[next_term]);
}

void map_vga_current(){
    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    reload_tlb();
}

void map_vga_scheduled(){
    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
//    setX(terminal_x[current_terminal]);
//    setY(terminal_y[current_terminal]);
    reload_tlb();
}

void unmap_vga_current() {
//    terminal_x[active_terminal] = getX();
//    terminal_y[active_terminal] = getY();
    uint32_t idx = page_entry(VGA_TEXT_BUF_ADDR);
    page_table0[idx].base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
//    setX(terminal_x[current_terminal]);
//    setY(terminal_y[current_terminal]);
    reload_tlb();
}
