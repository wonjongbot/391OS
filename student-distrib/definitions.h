//
// Created by hijoe on 4/29/2023.
//

#ifndef MP3_DEFINITIONS_H
#define MP3_DEFINITIONS_H

#define kb_buf_size 128
#define kb_history_size 128
#define screen_w 80
#define terminal_count 3
#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB_DEFAULT      0x7
#define VGA_TEXT_BUF_ADDR        0xB8000
#define VGA_TERM_0               0xB9000
#define VGA_SIZE                  (1 << 12)
#define BOTTOM          (1 << 23)
#define PCB_size        (1 << 13)
#define esp0(pid) (BOTTOM - PCB_size * pid - 4)

#endif //MP3_DEFINITIONS_H
