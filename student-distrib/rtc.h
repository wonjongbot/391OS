/* rtc.h - Defines functions and constant variables for rtc.c 
 */

#ifndef _RTC_H
#define _RTC_H

#define RTC             0x70    
#define CMOS            0x71
#define REG_A           0x8A
#define REG_B           0x8B
#define REG_C           0x8C
#define RTC_IRQ         8

/* Initializes RTC */
void rtc_init(void);

/* Handles interrupts */
void rtc_handler(void);

#endif