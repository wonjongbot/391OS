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

void rtc_set_freq(int32_t);

/* Handles interrupts */
void rtc_handler(void);

#endif
