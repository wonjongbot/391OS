//
// Created by hijoe on 4/29/2023.
//

#ifndef MP3_SCHEDULE_H
#define MP3_SCHEDULE_H

#include "types.h"
#include "lib.h"
#include "definitions.h"
#include "terminal.h"
#include "x86_desc.h"

int32_t active_terminal_idx;
uint32_t sched_terminal_idx;
int32_t active_process_idx;

void init_scheduler();

void schedule();

void context_switch(uint32_t idx);

void terminal_switch(uint32_t idx);

void print_proc();

#endif //MP3_SCHEDULE_H
