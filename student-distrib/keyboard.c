#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

void keyboard_init(){
    enable_irq(0x1);
    printf("bla");
}

void keyboard_handler(){
    disable_irq(0x1);
    printf("Handling keyboard\n");
    enable_irq(0x1);
    send_eoi(0x1);
}