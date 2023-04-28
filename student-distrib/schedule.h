#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "paging.h"
#include "rtc.h"

void schedule();
void terminal_switch(uint32_t to);
void context_switch(uint32_t to);

#endif
