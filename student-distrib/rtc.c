/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"


void rtc_init(void) {
    unsigned long flags;

    /* Turning on periodic interrupt: IRQ8 */
    cli_and_save(flags);    // Disable interrupts
    outb(REG_B | 0x80, RTC);       // Select Register B and disable NMI
    char prev = inb(CMOS);  // Read current value of register B
    outb(REG_B | 0x80, RTC);       // Set index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS);    // Write previous value ORed with 0x40. This turns on bit 6 of register B
    restore_flags(flags);

    enable_irq(RTC_IRQ);          // Turn on IRQ8
}

void rtc_set_freq(int32_t rate){
    unsigned long flags;
    /* Changing Interrupt Rate */
    rate &= 0x0F;   // rate must be above 2 and not over 15
    cli_and_save(flags);
    outb(REG_A | 0x80, RTC);   // Set index to register A, disable NMI
    char prev_2 = inb(CMOS);  // Get initial value of register A
    outb(REG_A | 0x80, RTC);   // Reset index to A
    outb((prev_2 & 0xF0) | rate, CMOS);   // Write only our rate to A. Rate is bottom 4 bits
    restore_flags(flags);
}

void rtc_handler(void) {

    /* Need these two lines or interrupt will not happen again to get another interrupt*/
    unsigned long flags;
    cli_and_save(flags);    // Disable interrupts

    outb(REG_C, RTC);	// select register C
    inb(CMOS);		    // just throw away contents
    send_eoi(RTC_IRQ);

    putc_rtc();

    restore_flags(flags);
}
