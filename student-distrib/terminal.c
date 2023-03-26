#include "terminal.h"

uint32_t terminal_open(){
    return 0;
}

uint32_t terminal_read(uint32_t fd, uint8_t* buf, uint32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        while(enter_flag == 0);
        if(kb_buf_top == 127){
            push_kb_buf('\n');
            putc('\n');
        }
        while(i < nbytes && kb_buf_top < 128){
            buf[i] = kb_buf[i];
            i++;
        }
        if(nbytes < kb_buf_top){
            buf[nbytes - 1] = '\n';
        }
        clear_kb_buf();
        return i;
    }
}

uint32_t terminal_write(uint32_t fd, const uint8_t* buf, uint32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        while(i < nbytes){
            putc(buf[i]);
            i++;
        }
        return nbytes;
    }
}

uint32_t terminal_close(uint32_t fd){
    return 0;
}
