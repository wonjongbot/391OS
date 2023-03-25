/* rtc.h - Defines functions and constant variables for rtc.c 
 */

#ifndef _RTC_H
#define _RTC_H

#define RTC             0x70    
#define CMOS            0x71
#define REG_A           0xA
#define REG_B           0xB
#define REG_C           0xC
#define RTC_IRQ         0x8

/* Initializes RTC */
void rtc_init(void);

/* Set RTC Frequency */
void rtc_set_rate(unsigned rate);

/* Helper function to check for power of 2 */
int32_t pow_of_two(unsigned number);

/* Set RTC Rate */
int32_t rtc_set_freq(unsigned frequency);

/* Handles interrupts */
void rtc_handler(void);

/* opens RTC */
int32_t rtc_open (const uint8_t* filename);

/* read RTC */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes);

/* write RTC */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

/* close RTC */
int32_t rtc_close (int32_t fd);
#endif
