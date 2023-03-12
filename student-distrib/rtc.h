/* rtc.h - Defines functions and constant variables for rtc.c 
 */

#ifndef _RTC_H
#define _RTC_H

#define REG_NUM    0x70    // Disables NMI
#define REG_A          0xA
#define REG_B          0xB
#define REG_C          0xC

/* Initializes RTC */
void rtc_init(void);

/* Handles interrupts */
void rtc_handler(void);

#endif