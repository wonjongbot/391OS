/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0

#define MASTER_COMMAND      MASTER_8259_PORT 
#define MASTER_DATA         MASTER_8259_PORT + 1
#define SLAVE_COMMAND       SLAVE_8259_PORT 
#define SLAVE_DATA          SLAVE_8259_PORT + 1
#define MASK                0xFF

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11    // start init, edge-triggered inputs, cascade mode, 4 ICWs
#define ICW2_MASTER         0x20    // high bits of vector, vector offset
#define ICW2_SLAVE          0x28    // high bits of vector, vector offset
#define ICW3_MASTER         0x04    // bit vector of seconday PIC, Tell it how it is wired to master/slaves
#define ICW3_SLAVE          0x02    // input pin on primary PIC, Tell it how it is wired to master/slaves
#define ICW4                0x01    // ISA=x86, normal/auto EOI, Gives additional information about the environment

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
