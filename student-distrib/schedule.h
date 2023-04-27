#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "system_calls.h"
#include "pcb.h"
#include "filesystem.h"
#include "paging.h"
#include "rtc.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "pcb.c"


// void init_multiterm();

// physical addresses for the three terminals
#define uprog_paddr0 (1<<23)
#define uprog_paddr1 (1<<24)
#define uprog_paddr2 (1<<25)

#endif MULTITERM_H