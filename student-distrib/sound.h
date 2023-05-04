#ifndef _SOUND_H_
#define _SOUND_H_

#include "lib.h"


#define PIT_FRQ 1193180
#define PIT_CH2 0x42
#define PIT_CMD 0x43
#define PIT_SOUND 0xb6
// 0xb6 = 1011 0110
// first 10 means chanel 2, 11 means using both high 8 bits and low 8 bits
// as count, 011 means using mode 3 (square wave mode), last 0 means binary mode
#define PIT_INPUT 0x61



void play_sound ( uint32_t nFrequence );
void stop_sound ();
void beep();


#endif
