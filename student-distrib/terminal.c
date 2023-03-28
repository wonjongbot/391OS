#include "terminal.h"

/* void terminal_history_handler();
 * Inputs: none;
 * Return Value: none
 * Function: handles terminal history functioanlity.
 *           up arrow shows previous terminal command and vice versa
 */
void terminal_history_handler(){
    int i;
    if(up_flag == 1 && kb_buf_history_top > 0 && kb_buf_history_ptr > 0){
        if(kb_buf_history_ptr == kb_buf_history_top){
            memcpy(kb_buf_history[kb_buf_history_top], kb_buf, kb_buf_size);
            kb_buf_top_cached = kb_buf_top;
        }
        if(kb_buf_top > screen_w - 1){
            while(kb_buf_top > screen_w - 1){
                putc('\b');
                pop_kb_buf();
            }
        }
        kb_buf_history_ptr--;
        memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], kb_buf_size);
        i = 0;
        clear_line();
        while(1){
            if(kb_buf[i] == '\n' || kb_buf[i] == '\r'){
                break;
            }
            putc(kb_buf[i]);
            i++;
        }
        kb_buf_top = i;
        up_flag = 0;
        down_flag = 0;
    }
    else if(down_flag == 1 && kb_buf_history_ptr < kb_buf_history_top && kb_buf_history_top < kb_history_size){
        kb_buf_history_ptr++;
        if(kb_buf_top > screen_w - 1){
            while(kb_buf_top > screen_w - 1){
                putc('\b');
                pop_kb_buf();
            }
        }
        if(kb_buf_history_ptr == kb_buf_history_top){
            i = 0;
            clear_line();
            memcpy(kb_buf, kb_buf_history[kb_buf_history_top], kb_buf_size);
            kb_buf_top = kb_buf_top_cached;
            while(i < kb_buf_top_cached){
                putc(kb_buf[i]);
                i++;
            }
        }
        else{
            memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], kb_buf_size);
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

/* void terminal_init();
 * Inputs: none;
 * Return Value: none
 * Function: initializes terminal history stack pointer and index to 0.
 */
void terminal_init(){
    kb_buf_history_ptr = 0;
    kb_buf_history_top = 0;
}

/* int32_t terminal_open();
 * Inputs: none
 * Return Value: -1
 * Function: returns -1 because terminal shouldn't be open in cp2
 */
int32_t terminal_open(){
    return -1;
}

/* int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes);
 * Inputs: int32_t fd, int8_t* buf, int32_t nbytes
 * Return Value: -1 for invalid ptr, num bytes read if else
 * Function: reads from keyboard buffer to input buffer for nbytes of time 
 *              or up til 128 bytes. Last byte read is always newline char.
 */
int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        for(i = 0; i<nbytes; i++){
            buf[i] = 0;
        }
        i = 0;
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
        while(i < nbytes && kb_buf_top <= kb_buf_size){
            buf[i] = kb_buf[i];
            if(buf[i] == '\n' || buf[i] == '\r'){
                i++;
                break;
            }
            i++;
        }
        buf[nbytes - 1] = '\n';
        #if ENABLE_HISTORY
        memcpy((kb_buf_history[kb_buf_history_top]), kb_buf, kb_buf_size);
        kb_buf_history_top++;
        #endif
        clear_kb_buf();
        enter_flag = 0;
        return i;
    }
}

/* int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes);
 * Inputs: int32_t fd, int8_t* buf, int32_t nbytes
 * Return Value: -1 for invalid ptr, num bytes written to screen if else
 * Function: reads from input buffer and displays that onto scren by copying
 *           nbytes of ascii data into vram. 
 */
int32_t terminal_write(int32_t fd, const int8_t* buf, int32_t nbytes){
    uint32_t i = 0;
    if(buf == NULL){
        return -1;
    }
    else{
        while(i < nbytes){
            putc(buf[i]);
            i++;
        }
        return i;
    }
}

/* int32_t terminal_close(int32_t fd);
 * Inputs: none;
 * Return Value: -1
 * Function: terminal shouldn't be closed for cp2.
 */
int32_t terminal_close(int32_t fd){
    return -1;
}
