/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"


void init_rtc(void) {
    unsigned long flags;

    /* Turning on periodic interrupt: IRQ8 */
    cli_and_save(flags);    // Disable interrupts
    outb(REG_B, RTC);       // Select Register B and disable NMI
    char prev = inb(CMOS);  // Read current value of register B
    outb(REG_B, RTC);       // Set index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS);    // Write previous value ORed with 0x40. This turns on bit 6 of register B
    restore_flags(flags);
    enable_irq(RTC_IRQ);          // Turn on IRQ8

    /* Changing Interrupt Rate */
    int32_t rate = 0;   // Not sure what to init the rate to
    rate &= 0x0F;   // rate must be above 2 and not over 15
    cli_and_save(flags);
    outb(REG_A, RTC);   // Set index to register A, disable NMI
    char prev_2 = inb(CMOS);  // Get initial value of register A
    outb(REG_A, RTC);   // Reset index to A
    outb((prev_2 & 0xF0) | rate, CMOS);   // Write only our rate to A. Rate is bottom 4 bits
    restore_flags(flags);
}

void handler_rtc(void) {

    /* Need these two lines or interrupt will not happen again to get another interrupt*/
    outb(REG_C, RTC);	// select register C
    inb(CMOS);		    // just throw away contents
    
    send_eoi(RTC_IRQ);
}