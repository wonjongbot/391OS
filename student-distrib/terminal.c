#include "terminal.h"

/* void terminal_history_handler();
 * Inputs: none;
 * Return Value: none
 * Function: handles terminal history functioanlity.
 *           up arrow shows previous terminal command and vice versa.
 *           user can store up til 128 terminal history.
 */
void terminal_history_handler(){
    int i;
    // we can only access 128 histories
    if(kb_buf_top[active_terminal] == 0 &&(up_flag == 1 && kb_buf_history_top > 0 && kb_buf_history_ptr > 0)){
        // if up was pressed in the top of keyboard history, we copy
        // what we have so far into the most recent history and save num chars
        if(kb_buf_history_ptr == kb_buf_history_top){
            memcpy(kb_buf_history[kb_buf_history_top], kb_buf[active_terminal], kb_buf_size);
            kb_buf_top_cached = kb_buf_top[active_terminal];
        }
        // if current terminal argument is longer than screen width, delete stuff
        // so that cursor is in the first line
        if(kb_buf_top[active_terminal] > screen_w - 1){
            while(kb_buf_top[active_terminal] > screen_w - 1){
                putc('\b');
                pop_kb_buf();
            }
        }
        // point at next most recent history
        kb_buf_history_ptr--;
        // copy contents of that history buffer to keyboard buffer
        memcpy(kb_buf[active_terminal], kb_buf_history[kb_buf_history_ptr], kb_buf_size);
        i = 0;
        // print the saved buffer to screen, after clearing current line.
        clear_line();
        //terminal_write(1, (void*)"391OS> ",7);
        while(1){
            if(kb_buf[active_terminal][i] == '\n' || kb_buf[active_terminal][i] == '\r'){
                break;
            }
            putc(kb_buf[active_terminal][i]);
            i++;
        }
        kb_buf_top[active_terminal] = i;
        up_flag = 0;
        down_flag = 0;
    }
    else if(down_flag == 1 && kb_buf_history_ptr < kb_buf_history_top && kb_buf_history_top < kb_history_size){
        kb_buf_history_ptr++;
        if(kb_buf_top[active_terminal] > screen_w - 1){
            while(kb_buf_top[active_terminal] > screen_w - 1){
                putc('\b');
                pop_kb_buf();
            }
        }
        // if we are printing the in-progress buffer, there is no newline char
        // to terminate on. Use cached buffer length as a terminator.
        if(kb_buf_history_ptr == kb_buf_history_top){
            i = 0;
            clear_line();
            // terminal_write(1, (void*)"391OS> ",7);
            memcpy(kb_buf[active_terminal], kb_buf_history[kb_buf_history_top], kb_buf_size);
            kb_buf_top[active_terminal] = kb_buf_top_cached;
            while(i < kb_buf_top_cached){
                putc(kb_buf[active_terminal][i]);
                i++;
            }
        }
        else{
            // for other saved buffers you just print until newline after clearing line.
            // same thing as above.
            memcpy(kb_buf[active_terminal], kb_buf_history[kb_buf_history_ptr], kb_buf_size);
            i = 0;
            clear_line();
            // terminal_write(1, (void*)"391OS> ",7);
            while(1){
                if(kb_buf[active_terminal][i] == '\n' || kb_buf[active_terminal][i] == '\r'){
                    kb_buf_top[active_terminal] = ++i;
                    break;
                }
                putc(kb_buf[active_terminal][i]);
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

// TODO the header for this
/* int32_t terminal_open();
 * Inputs: none
 * Return Value: -1
 * Function: returns -1 because terminal shouldn't be open in cp2
 */
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes) {
  return terminal_read(fd, buf, nbytes);
}

int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes) {
  return terminal_write(fd, buf, nbytes);
}

int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;
}

int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes) {
  return -1;
}

/* int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes);
 * Inputs: int32_t fd, int8_t* buf, int32_t nbytes
 * Return Value: -1 for invalid ptr, num bytes read if else
 * Function: reads from keyboard buffer to input buffer for nbytes of time
 *              or up til 128 bytes. Last byte read is always newline char.
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    uint32_t i = 0;
    int8_t* buf_c = (int8_t*) buf;
    // check for nullptr
    if(buf_c == NULL) {
        return -1;
    }
    else{
        // flush the input buffer just in case
        for(i = 0; i<nbytes; i++){
            buf_c[i] = 0;
        }
        i = 0;
        #if ENABLE_HISTORY
        kb_buf_history_ptr = kb_buf_history_top;
        #endif
        // wait for enter key or newline char to be entered.
        while(enter_flag == 0){
            #if ENABLE_HISTORY
            terminal_history_handler();
            #endif
        }
        // copy whatever is in the keyboard buffer to the input buffer, nbytes number of
        // times. including newline character
        i = 0;
        while(i < nbytes && kb_buf_top[active_terminal] <= kb_buf_size){
            buf_c[i] = kb_buf[active_terminal][i];
            if(buf_c[i] == '\n' || buf_c[i] == '\r'){
                buf_c[i] = '\n';
                i++;
                break;
            }
            i++;
        }
        // for the case of read being stopped in the middle, insert newline char in the end of read
        buf_c[nbytes - 1] = '\n';
        // copy what we have in keyboard buffer into most recent keyboard buffer
        #if ENABLE_HISTORY
        memcpy((kb_buf_history[kb_buf_history_top]), kb_buf[active_terminal], kb_buf_size);
        kb_buf_history_top++;
        #endif
        // clear keyboard buffer for next read operation.
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
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    uint32_t i = 0;
    if((int8_t*)buf == NULL){
        return -1;
    }
    else{
        // write to screen nbytes amount of time.
        while(i < nbytes){
            putc(*((int8_t*)buf+i));
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
    return 0;
}
