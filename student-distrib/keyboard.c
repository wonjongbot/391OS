#include "keyboard.h"
#include "i8259.h"

void keyboard_init(){
    enable_irq(0x1);
}