/* .h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "types.h"

#define ENABLE_HISTORY 1
#define TERMINAL_PROMPT_MODE 0
#define TERMINAL_PROMPT_LEN 6
#define kb_buf_size 128
#define kb_history_size 128
#define screen_w 80

char kb_buf_history[kb_history_size][kb_buf_size];
int kb_buf_history_ptr;
int kb_buf_history_top;
int kb_buf_top_cached;

void terminal_init();

int32_t terminal_open();

int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes);

int32_t terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes);

int32_t terminal_close(int32_t fd);

#endif
