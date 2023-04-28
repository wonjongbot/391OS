#include "pit.h"

void pit_init(void){
    int counter = 1193182 / 100;
    outb(PIT_MODE2_LOHI, PIT_CMD);
    //0x36
    outb(counter & 0xff, PIT_CH_0);
    outb(counter >> 8, PIT_CH_0);

    enable_irq(PIT_IRQ);
}

void pit_handler(){
    // schedule();
    // printf("pit");
    send_eoi(PIT_IRQ);
}
