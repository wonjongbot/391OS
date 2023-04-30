//
// Created by hijoe on 4/29/2023.
//

#ifndef MP3_PIT_H
#define MP3_PIT_H

#include "i8259.h"
#include "paging.h"
#include "schedule.h"

// PIT channels
#define PIT_CH_0    0x40
#define PIT_CH_1    0x41
#define PIT_CH_2    0x42
#define PIT_CMD     0x43

#define PIT_MODE2_LOHI 0x34

#define PIT_IRQ     0x0

#include "types.h"
#include "schedule.h"

void pit_init();

void pit_handler();

#endif //MP3_PIT_H
