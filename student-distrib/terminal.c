#include "terminal.h"


void terminal_history_handler(){
    int i;
    if(up_flag == 1 && kb_buf_history_top > 0 && kb_buf_history_ptr > 0){
        if(kb_buf_history_ptr == kb_buf_history_top){
            memcpy(kb_buf_history[kb_buf_history_top], kb_buf, 128);
            kb_buf_top_cached = kb_buf_top;
        }
        kb_buf_history_ptr--;
        memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], 128);
        i = 0;
        clear_line();
        while(1){
            if(kb_buf[i] == '\n' || kb_buf[i] == '\r'){
                kb_buf_top = ++i;
                break;
            }
            putc(kb_buf[i]);
            i++;
        }
        up_flag = 0;
        down_flag = 0;
    }
    else if(down_flag == 1 && kb_buf_history_ptr < kb_buf_history_top && kb_buf_history_top < kb_history_size){
        kb_buf_history_ptr++;
        if(kb_buf_history_ptr == kb_buf_history_top){
            i = 0;
            clear_line();
            memcpy(kb_buf, kb_buf_history[kb_buf_history_top], 128);
            kb_buf_top = kb_buf_top_cached;
            while(i < kb_buf_top_cached){
                putc(kb_buf[i]);
                i++;
            }
        }
        else{
            memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], 128);
            i = 0;
            clear_line();
            while(1){
                if(kb_buf[i] == '\n' || kb_buf[i] == '\r'){
                    kb_buf_top = ++i;
                    break;
                }
                putc(kb_buf[i]);
                i++;
            }
        }
        // if I don't clear both flags, it will ignore the key (or goes up and then down so it looks like
        // if ignores key) when ptr is on the edge idk 
        up_flag = 0;
        down_flag = 0;
    }
}

void terminal_init(){
    kb_buf_history_ptr = 0;
    kb_buf_history_top = 0;
}

int32_t terminal_open(){
    return 0;
}

int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        #if ENABLE_HISTORY
        kb_buf_history_ptr = kb_buf_history_top;
        #endif
        while(enter_flag == 0){
            #if ENABLE_HISTORY
            terminal_history_handler();
            #endif
        }
        // push current reads to history
        i = 0;
        while(i < nbytes && kb_buf_top <= 128){
            buf[i] = kb_buf[i];
            if(buf[i] == '\n' || buf[i] == '\r'){
                i++;
                break;
            }
            i++;
        }
        buf[nbytes - 1] = '\n';
        #if ENABLE_HISTORY
        memcpy((kb_buf_history[kb_buf_history_top]), kb_buf, 128);
        kb_buf_history_top++;
        #endif
        clear_kb_buf();
        enter_flag = 0;
        return i;
    }
}

int32_t terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        while(i < nbytes){
            putc(buf[i]);
            if(buf[i] == '\n' || buf[i] == '\r'){
                i++;
                break;
            }
            i++;
        }
        return i;
    }
}

int32_t terminal_close(int32_t fd){
    return -1;
}
