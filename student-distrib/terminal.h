/* .h - Defines used in interactions with the ps/2 keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "types.h"

void terminal_init();

uint32_t terminal_open();

uint32_t terminal_read(uint32_t fd, uint8_t* buf, uint32_t nbytes);

uint32_t terminal_write(uint32_t fd, const uint8_t* buf, uint32_t nbytes);

uint32_t terminal_close(uint32_t fd);

#endif
