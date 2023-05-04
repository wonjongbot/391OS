#include "terminal.h"

/* void terminal_init();
 * Inputs: none;
 * Return Value: none
 * Function: initializes terminal history stack pointer and index to 0.
 */
void terminal_init(){
    int i;
    for (i = 0; i < 3; i++) {
        kb_buf_history_ptr[i] = 0;
        kb_buf_history_top[i] = 0;
    }
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
        // wait for enter key or newline char to be entered.
        while(enter_flag[current_terminal] == 0){
//            #if ENABLE_HISTORY
//            terminal_history_handler();
//            #endif
        }
#if ENABLE_HISTORY
        kb_buf_history_ptr[current_terminal] = kb_buf_history_top[current_terminal];
#endif
        // copy whatever is in the keyboard buffer to the input buffer, nbytes number of
        // times. including newline character
        i = 0;
        while(i < nbytes && kb_buf_top[current_terminal] <= kb_buf_size){
            buf_c[i] = kb_buf[current_terminal][i];
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
        if (buf_c[0] != '\n') {
            memcpy(kb_buf_history[current_terminal][kb_buf_history_top[current_terminal]], kb_buf[current_terminal], kb_buf_size);
            kb_buf_history_top[current_terminal]++;
            kb_buf_history_ptr[current_terminal] = kb_buf_history_top[current_terminal];
        }
        #endif
        // clear keyboard buffer for next read operation.
        clear_kb_buf();
        enter_flag[current_terminal] = 0;
        terminal_x[current_terminal] = getX();
        terminal_y[current_terminal] = getY();
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
        if(current_terminal == active_terminal)
            map_vga_current();
        else
            map_vga_scheduled();
        while(i < nbytes){
            putc(*((int8_t*)buf+i));
            i++;
        }
        terminal_x[current_terminal] = getX();
        terminal_y[current_terminal] = getY();
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
