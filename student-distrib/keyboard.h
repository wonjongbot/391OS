/* keyboard.h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "definitions.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "system_calls.h"


// macro for special keys. DOWN keys by default
// KEY_UP macro caculates UP key of the input key
#define KEY_UP(key) key | 0x80
#define LSHIFT 0x2a
#define RSHIFT 0x36
#define ALT 0x38
#define CAPS 0x3A
#define CTRL 0x1D
#define UP  0x48
#define DOWN 0x50
#define LEFT 0x4B
#define RIGHT 0x4d
#define ENTER 0x1c
#define TAB_SIZE 4

#define CASE_OFFSET 0x20
#define UPPERCASE(ASCII) ASCII - CASE_OFFSET

// think of this array as a stack where top is next availble location
char kb_buf[kb_buf_size];
int kb_buf_top;
int caps_flag;
int shift_flag;
int alt_flag;
int ctrl_flag;
int enter_flag;
int up_flag;
int down_flag;
int right_flag;
int left_flag;

void push_kb_buf(uint8_t ascii);

uint8_t pop_kb_buf();

void clear_kb_buf();

void keyboard_init();

void keyboard_handler();

#endif
