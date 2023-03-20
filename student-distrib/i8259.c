/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* 
 * i8259_init
 * Input: None
 * Output: None
 * Initialize the 8259 PIC
 */
void i8259_init(void) {    
    unsigned long flags;

    cli_and_save(flags);    // Disable interupts

    master_mask = inb(MASTER_DATA);
    slave_mask = inb(SLAVE_DATA);

    outb(MASK, MASTER_DATA);
    outb(MASK, SLAVE_DATA);
 
	outb(ICW1, MASTER_COMMAND);  // starts the initialization sequence (in cascade mode)

	outb(ICW1, SLAVE_COMMAND);

	outb(ICW2_MASTER, MASTER_DATA);     // ICW2: Master PIC vector offset
	
	outb(ICW2_SLAVE, SLAVE_DATA);       // ICW2: Slave PIC vector offset

	outb(ICW3_MASTER, MASTER_DATA);     // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	
	outb(ICW3_SLAVE, SLAVE_DATA);       // ICW3: tell Slave PIC its cascade identity (0000 0010)

 
	outb(ICW4, MASTER_DATA);    // Set 8086 Mode

	outb(ICW4, SLAVE_DATA);     // Set 8086 Mode

	outb(0xff, MASTER_DATA);   // restore saved masks.
	outb(0xff, SLAVE_DATA);

    restore_flags(flags);

    // enable secondary pic for interrupts
    enable_irq(0x2);
}

/* 
 * enable_irq
 * Input: None
 * Output: None
 * Enable (unmask) the specified IRQ 
 */
void enable_irq(uint32_t irq_num) {
    unsigned long flags;

    cli_and_save(flags);    // Disable interupts

    uint16_t port;
    uint8_t value;

    if (irq_num < 8) {
        port = MASTER_DATA;
    } else {
        port = SLAVE_DATA;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);
    outb(value, port);

    restore_flags(flags);
}

/* 
 * disable_irq
 * Input: None
 * Output: None
 * Disable (mask) the specified IRQ
 */
void disable_irq(uint32_t irq_num) {
    unsigned long flags;

    cli_and_save(flags);    // Disable interupts

    uint16_t port;
    uint8_t value;

    if (irq_num < 8) {
        port = MASTER_DATA;
    } else {
        port = SLAVE_DATA;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);
    outb(value, port);

    restore_flags(flags);
}

/* 
 * send_eoi
 * Input: None
 * Output: None
 * Send end-of-interrupt signal for the specified IRQ
 */
void send_eoi(uint32_t irq_num) {
    unsigned long flags;

    cli_and_save(flags);    // Disable interupts

    if (irq_num >= 8)
        outb(EOI, SLAVE_COMMAND);
    outb(EOI, MASTER_COMMAND);

    
    restore_flags(flags);
}
