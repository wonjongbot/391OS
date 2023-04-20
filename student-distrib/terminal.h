/* .h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define muti_terminal 0

#include "lib.h"
#include "keyboard.h"
#include "types.h"

#define ENABLE_HISTORY 0
#define TERMINAL_PROMPT_MODE 0
#define TERMINAL_PROMPT_LEN 7
#define kb_buf_size 128
#define kb_history_size 128
#define screen_w 80

#define TERMINAL_TEXT_COLOR 0x3C
#define TERMINAL_BACKGROUND_COLOR 0x03
#define TERMINAL_NUM 3
#define TERMINAL_INPUT_BUF_SIZE 128
#define TERMINAL_TEXT_BUF_SIZE 2100

char kb_buf_history[kb_history_size][kb_buf_size];
int kb_buf_history_ptr;
int kb_buf_history_top;
int kb_buf_top_cached;

#if (muti_terminal==1)
typedef struct terminal{
    int start_row_index;
    int cur_x;
    int cur_y;
    unsigned char text_color;
    unsigned char background_color;
    
    char* text_buf;         // can not be modified after driver init
    int input_buf_cur_pos;
    char* input_buf;        // can not be modified after driver init
    
} terminal;
typedef struct terminal terminal_struct;


static char input_bufs[TERMINAL_NUM][TERMINAL_INPUT_BUF_SIZE ];
static char text_bufs[TERMINAL_NUM][TERMINAL_TEXT_BUF_SIZE];
static terminal terminal_arr[TERMINAL_NUM];
static int terminal_used[TERMINAL_NUM];

int terminal_init_each(terminal* terminal, char* text_buf, char* input_buf);
#endif

void terminal_init();


int32_t terminal_open(const uint8_t* filename);

int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t terminal_close(int32_t fd);

int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes);

#if (muti_terminal==1)
void terminal_buf_put(terminal* terminal, char c);
void terminal_buf_delete(terminal* terminal);
void terminal_buf_clear(terminal* terminal);

terminal* terminal_alloc();    // dynamic terminal allocator
int terminal_free(terminal* terminal);    // free the dynamic allocated terminal
terminal* terminal_get(int index);
int terminal_get_index(terminal* terminal);
#endif

#endif
