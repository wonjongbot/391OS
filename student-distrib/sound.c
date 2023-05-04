#include "sound.h"

#define SOUND_BIT		3
#define SOUND_SHIFT	8
#define SOUND_SHIFT_HIGH	16
#define SOUND_LOW_MASK 0xFFFF
#define SOUND_MASK		0xfc




/* 
 *   play_sound
 *   DESCRIPTION: play a sound along an frequency
 *   INPUTS: count
 *   OUTPUTS: none 
 *   RETURN VALUE:  none
 *   SIDE EFFECTS: produce a sound through the speaker
 */
void play_sound ( uint32_t count )  { 
 	uint16_t frq ; 
 	//uint8_t temp ;
    uint8_t data;
 
     //set the frequency
 	frq = PIT_FRQ /count; 
 	outb (PIT_SOUND,PIT_CMD); 
    
    uint8_t low = (uint8_t)(frq);// 1011 0110
    uint8_t high = (uint8_t)(frq>>SOUND_SHIFT);

 	outb (low,PIT_CH2); 
 	outb (high,PIT_CH2);
 
    //use the speaker to speak
    //uint8_t shift = temp|SOUND_BIT;
 	data = inb(PIT_INPUT)|SOUND_BIT; 
    outb (data,PIT_INPUT) ;
  	//if (temp != shift){ 
 	//	outb (shift,PIT_INPUT) ; 
 	//} 


 }

 /* 
 *   stop_sound
 *   DESCRIPTION: mask the sound
 *   INPUTS: none
 *   OUTPUTS: none 
 *   RETURN VALUE:  none
 *   SIDE EFFECTS: none
 */
void stop_sound ()  { 
 	uint8_t temp = inb(PIT_INPUT)&SOUND_MASK;
 	outb ( temp, PIT_INPUT); 

 }

 /* 
 *   beep()
 *   DESCRIPTION: produce a beep
 *   OUTPUTS: none 
 *   RETURN VALUE:  none
 *   SIDE EFFECTS: produce a beep
 */
void beep() {
    //printf("OKK\n");
 	 play_sound(1000);
    int i = 0;
    for(i=0;i<100000000;i++){}
 	 stop_sound();

 }






