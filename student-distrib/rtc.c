/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"

#define SQUASH_TEST 0

/* Global Variables */
unsigned rtc_counter = 0;   // Keeps track of number of interrupts generated by RTC
unsigned rtc_target = 1;    // To keep track if counter > this value. Gives number of interrupts
volatile int interrupt_flag = 0;     // interrupt is off

/* 
 * rtc_init
 * Input: None
 * Output: None
 * Starts up the rtc in periodic interrupt mode
 */
void rtc_init(void) {
    // unsigned long flags;

    /* Turning on periodic interrupt: IRQ8 */
    // cli_and_save(flags);    // Disable interrupts
    outb(REG_B | 0x80, RTC);       // Select Register B and disable NMI
    char prev = inb(CMOS);  // Read current value of register B
    outb(REG_B | 0x80, RTC);       // Set index again (a read will reset the index to register D)
    outb(prev | 0x40, CMOS);    // Write previous value ORed with 0x40. This turns on bit 6 of register B
    // restore_flags(flags);

    // set the rtc to fastest frequency possible: 1024 Hz
    rtc_set_rate(6);

    enable_irq(RTC_IRQ);          // Turn on IRQ8
}

/* 
 * rtc_set_rate
 * Input: None
 * Output: None
 * Set RTC Rate
 */
void rtc_set_rate(unsigned rate){
    //unsigned long flags;
    /* Changing Interrupt Rate */
    rate &= 0x0F;   // rate must be above 2 and not over 15
    //cli_and_save(flags);
    outb(REG_A | 0x80, RTC);   // Set index to register A, disable NMI
    char prev_2 = inb(CMOS);  // Get initial value of register A
    outb(REG_A | 0x80, RTC);   // Reset index to A
    outb((prev_2 & 0xF0) | rate, CMOS);   // Write only our rate to A. Rate is bottom 4 bits
    //restore_flags(flags);
}


/* 
 * rtc_set_freq
 * Input: None
 * Output: None
 * Set RTC Frequency
 */
int32_t rtc_set_freq(unsigned frequency){
    if(frequency == 0){
        rtc_set_rate(0);
    } else if (pow_of_two(frequency) == 0 || frequency < 2 || frequency > 1024) {
        return -1;
    }
    else{
        // rtc_set_rate(6);
        // reset the counter
        rtc_counter = 1;
        // set counter target
        rtc_target = (float)(1024) / (float)frequency;  // Gives number of interrupts. 1024 Hz
    }

    return 0;
}

/*
 * rtc_handler
 * Input: None
 * Output: None
 * Handles all RTC interrupts and runs through test interrupts
 */
void rtc_handler(void) {

    /* Need these two lines or interrupt will not happen again to get another interrupt*/
    unsigned long flags;
    cli_and_save(flags);    // Disable interrupts

    outb(REG_C, RTC);	// select register C
    inb(CMOS);		    // just throw away contents
    if(rtc_counter == 0){
        //putc_rtc();
        interrupt_flag = 1;
        //putc('.');
        //test_interrupts(); 
    }
    rtc_counter = (rtc_counter + 1)%rtc_target; 
    send_eoi(RTC_IRQ);
    restore_flags(flags);
    return;
}


/*
 * rtc_open
 * Input: None
 * Output: Returns 0
 * Initializes RTC frequency to 2 HZ
 */
int32_t rtc_open (const uint8_t* filename) {
    rtc_set_freq(2);    // Set frequency to 2 HZ
    return 0;
}


/*
 * rtc_read
 * Input: None
 * Output: None
 * Wait for next interrupt, then return 0. 
 * Not for reading RTC frequency
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes) {
    // wait until interrupt handler clears flag
    if(buf == NULL){
        return -1;
    }
    interrupt_flag = 0;
    while (interrupt_flag == 0);

    // Testing user program squashing on exception
    #if SQUASH_TEST
    int a;
    a = 0;
    a = 3 / a;
    printf("Triggering Exception%d\n",a);
    #endif

    interrupt_flag = 0; // interrupt is off
    return 0;
}

/*
 * rtc_write
 * Input: buf- the new frequency to be written
 * Output: Returns 0 on success, -1 on failure(invalid frequency)
 * Changes RTC frequency
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes) {
    /* Sanity checks done in rtc_set_freq */
    if(buf == NULL){
        return -1;
    }
    if(*(unsigned*)buf == 0){
        return -1;
    }
    return rtc_set_freq(*(unsigned*)buf);    // buf is the new frequency to be written

}

/*
 * rtc_close
 * Input: None
 * Output: None
 * Handles all RTC interrupts and runs through test interrupts
 */
int32_t rtc_close (int32_t fd) {
    return 0;
}


/* Helper Functions */
/* 
 * pow_of_two
 * Input: the frequency
 * Output: 1 for true, 0 for false
 * Dewtermine if the frequency is a power of 2
 */
int32_t pow_of_two(unsigned number) {
    if (number == 1) {
        return 1;   // true
    } 
    if (number == 0 || number % 2 != 0) {
        return 0;   // false
    }
    return pow_of_two(number/2);
}
