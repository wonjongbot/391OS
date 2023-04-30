#include "pit.h"

void pit_init(void){
    int counter = 1193182 / 100;
    outb(PIT_MODE2_LOHI, PIT_CMD);
    //0x36
    outb(counter & 0xff, PIT_CH_0);
    outb(counter >> 8, PIT_CH_0);

    enable_irq(PIT_IRQ);
}

void pit_handler() {
    send_eoi(PIT_IRQ);

    int previous_terminal;

    PTE temp;

    current_terminal++;
    if (current_terminal > 2) current_terminal = 0;

    if (next_terminal != -1) {
        previous_terminal = active_terminal;
        active_terminal = next_terminal;
        next_terminal = -1;
        switch_video_mem(previous_terminal, active_terminal);
    }

    int idx = dir_entry(VIDMAP_START_VIRTUAL_ADDR);
    temp.val = page_table1[idx].val;

    if (current_terminal == active_terminal) {
        temp.base_addr = VGA_TEXT_BUF_ADDR >> TABLE_ADDRESS_SHIFT;
    } else {
        temp.base_addr = (VGA_TERM_0 + current_terminal * VGA_SIZE) >> TABLE_ADDRESS_SHIFT;
    }

    page_table1[idx].val = temp.val;

    switch_running_task(terminal_pids[active_terminal]);
}
