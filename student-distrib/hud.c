//
// Created by Peter Lee on 5/4/23.
//

#include "hud.h"

void hud(){

    int8_t* teststring = "T1:          ";

    int8_t* tmp = teststring;
    int x =0 , y = 0;

    int i, j;

    for(i = 0; i < 3; i++){
        x = i*10;
        y = 0;
        j = 0;
        tmp = teststring;
        *(tmp + 1) = i + 0x30;
        while(j < 7 || *(tmp + 3 + j) == '\0'){
            *(tmp + 3 + j) = *(PCB(terminal_pids[i])->prog_name + j);
            j++;
        }
        j = 0;
        while(j < 9){
            if( i == active_terminal)
                putc_on_coord(*tmp, x, y, TERM_SELECTED_ATTRIB);
            else
                putc_on_coord(*tmp, x, y, UI_COLOR);
            x++;
            j++;
            tmp++;
        }
        putc_on_coord('|', x, y, UI_COLOR);
        x++;
    }
    i = 0;
    while( i < 80){
        putc_on_coord('-', i, 1, UI_COLOR);
        i++;
    }

    putc_on_coord('R', 62, 0, UI_COLOR);
    putc_on_coord('T', 63, 0, UI_COLOR);
    putc_on_coord('C', 64, 0, UI_COLOR);
    putc_on_coord(':', 65, 0, UI_COLOR);
    putc_on_coord((rtc_freqs[active_terminal]/1000)%10 + 0x30, 66, 0, UI_COLOR);
    putc_on_coord((rtc_freqs[active_terminal]/100)%10 + 0x30, 67, 0, UI_COLOR);
    putc_on_coord((rtc_freqs[active_terminal]/10)%10 + 0x30, 68, 0, UI_COLOR);
    putc_on_coord(rtc_freqs[active_terminal]%10 + 0x30, 69, 0, UI_COLOR);
    putc_on_coord('H', 70, 0, UI_COLOR);
    putc_on_coord('z', 71, 0, UI_COLOR);
    putc_on_coord(' ', 72, 0, UI_COLOR);

    putc_on_coord('(', 73, 0, UI_COLOR);

    putc_on_coord(terminal_x[active_terminal]/10 + 0x30, 74, 0, UI_COLOR);
    putc_on_coord(terminal_x[active_terminal]%10 + 0x30, 75, 0, UI_COLOR);

    putc_on_coord(',', 76, 0, UI_COLOR);

    putc_on_coord(terminal_y[active_terminal]/10 + 0x30, 77, 0, UI_COLOR);
    putc_on_coord(terminal_y[active_terminal]%10 + 0x30, 78, 0, UI_COLOR);

    putc_on_coord(')', 79, 0, UI_COLOR);
}
