#include "pit.h"

// global variable tracking number of shells that are executed as base shell
volatile int shells_initialized = -1;

/*
 * pit_init
 * Input: None
 * Output: None
 * Return value: none
 * Initialize PIT to operate in 100Hz counter mode
 */
void pit_init(void){

    // set the counter target of PIT so it sends interrupt at 100Hz.
    int counter = 1193182 / 100;
    outb(PIT_MODE2_LOHI, PIT_CMD);
    //0x36
    outb(counter & 0xff, PIT_CH_0);
    outb(counter >> 8, PIT_CH_0);

    enable_irq(PIT_IRQ);
}

/*
 * pit_handler
 * Input: None
 * Output: None
 * Return value: none
 * handler for the PIT. Initializes three shells.
 */
void pit_handler() {
    send_eoi(PIT_IRQ);

    switch(current_terminal){
        case -1:
            switch_running_task(0);
            break;
        case 0:
            switch_running_task(1);
            break;
        case 1:
            switch_running_task(2);
            break;
        case 2:
            switch_running_task(0);
            break;
        default: break;
    }
//
//    // If we don't have all the shells initilaized, clear off the screen and prepare for executing shell
//            // on that terminal
//    if (shells_initialized == -1) {
//        clear();
//        reset_text_cursor();
//    }
//    // This is mainly for executing three different instances of shell program onto three terminals
//    if (shells_initialized < 3) {
//        shells_initialized++;
//        syscall_execute((uint8_t*)"shell");
//        //switch_active_terminal(shells_initialized % 3); // 0, 1, 2, 0
//    }
//    else{
//        schedule();
//        return;
//    }
}
