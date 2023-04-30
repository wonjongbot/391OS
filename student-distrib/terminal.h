/* .h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "types.h"
#include "schedule.h"
#include "definitions.h"
#include "rtc.h"
#include "paging.h"

typedef struct terminal {
    int32_t x, y;
    uint8_t attrib;

    uint8_t _kb_buf[kb_buf_size];
    uint32_t _kb_buf_top;

    uint32_t rtc_target;
    uint32_t rtc_counter;

    uint32_t ebp;

    int32_t pid;
} terminal_t;

terminal_t terminals[terminal_count];

void terminal_init();

void terminal_init_each(uint32_t index);

void save_terminal(uint32_t index);
void restore_terminal(uint32_t index);

uint8_t* kb_buf_();
uint32_t* kb_buf_top_();

int32_t terminal_open(const uint8_t* filename);

int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t terminal_close(int32_t fd);

int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes);

#endif
