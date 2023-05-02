//
// Created by hijoe on 4/30/2023.
//

#ifndef MP3_SCHEDULE_H
#define MP3_SCHEDULE_H

#include "types.h"

extern volatile int active_terminal;
extern volatile int current_terminal;
extern volatile int next_terminal;
extern volatile int terminal_pids[3];

extern volatile int halt_flag;

void init_scheduler();

void switch_running_task(uint32_t terminal_to);

void switch_active_terminal(uint32_t pid);

void switch_video_mem(uint32_t curr_term, uint32_t next_terminal);

void map_vga_current();
void unmap_vga_current();

#endif //MP3_SCHEDULE_H
