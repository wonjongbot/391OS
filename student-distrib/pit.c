#include "pit.h"

volatile int shells_initialized = -1;

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

    if (shells_initialized == -1) {
        clear();
        reset_text_cursor();
    }
    if (shells_initialized < 3) {
        shells_initialized++;
        switch_active_terminal(shells_initialized % 3); // 0, 1, 2, 0
        return;
    }
}
