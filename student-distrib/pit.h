#ifndef _PIT_H
#define _PIT_H

// PIT channels
#define PIT_CH_0    0x40
#define PIT_CH_1    0x41
#define PIT_CH_2    0x42
#define PIT_CMD     0x43

#define PIT_MODE2_LOHI 0x34

//0b00110100

#define PIT_IRQ     0x0

/* Initializes PIT */
void pit_init(void);

void pit_handler();

#endif
