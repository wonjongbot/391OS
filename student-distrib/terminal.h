/* .h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "pcb.h"
#include "types.h"

#define ENABLE_HISTORY 0
#define TERMINAL_PROMPT_MODE 0
#define TERMINAL_PROMPT_LEN 7
#define kb_buf_size 128
#define kb_history_size 128
#define screen_w 80

char kb_buf_history[kb_history_size][kb_buf_size];
int kb_buf_history_ptr;
int kb_buf_history_top;
int kb_buf_top_cached;

int32_t curr_term_sched;
int32_t curr_term_displayed;

typedef struct terminal{
    int cur_x;
    int cur_y;
    uint8_t text_attrib;
    char* text_buf;         // can not be modified after driver init

    int rtc_target;
    int rtc_state;

    // esp and ebp for scheduling
    uint32_t esp_sched;
    uint32_t ebp_sched;

    // pid number of most recent program
    int32_t curr_pid;
} terminal_t;

#define TERMINAL_NUM 3

terminal_t terminal_arr[TERMINAL_NUM];


void terminal_init();

int32_t terminal_open(const uint8_t* filename);

int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t terminal_close(int32_t fd);

int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes);

#endif
