#include "terminal.h"

/* void terminal_init();
 * Inputs: none;
 * Return Value: none
 * Function: initializes terminal history stack pointer and index to 0.
 */
void terminal_init(){
    uint32_t i;
    for (i = 0; i < terminal_count; i++) {
        terminal_init_each(i);
    }
}

void terminal_init_each(uint32_t index) {
    terminals[index].attrib = ATTRIB_DEFAULT;
    uint32_t i;
    for(i = 0; i < kb_buf_size; i++){
        terminals[index]._kb_buf[i] = 0;
    }
    terminals[index].pid = -1;
//    map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + VGA_SIZE * index);
}

void save_terminal(uint32_t index) {
    terminals[index].x = getX();
    terminals[index].y = getY();
    terminals[index].rtc_counter = get_rtc_counter();
    terminals[index].rtc_target = get_rtc_target();
    terminals[index].attrib = get_attrib();
    map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TEXT_BUF_ADDR);
    map_4KB_page(VGA_TERM_0 + VGA_SIZE * index, VGA_TERM_0 + VGA_SIZE * index);
    memcpy((uint8_t*) (VGA_TERM_0 + VGA_SIZE * index), (uint8_t*) VGA_TEXT_BUF_ADDR, VGA_SIZE);
    map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TERM_0 + VGA_SIZE * index);
}

void restore_terminal(uint32_t index) {
    setX(terminals[index].x);
    setY(terminals[index].y);
    set_rtc_counter(terminals[index].rtc_counter);
    set_rtc_target(terminals[index].rtc_target);
    set_attrib(terminals[index].attrib);
    map_4KB_page(VGA_TEXT_BUF_ADDR, VGA_TEXT_BUF_ADDR);
    map_4KB_page(VGA_TERM_0 + VGA_SIZE * index, VGA_TERM_0 + VGA_SIZE * index);
    memcpy((uint8_t*) VGA_TEXT_BUF_ADDR, (uint8_t*) (VGA_TERM_0 + VGA_SIZE * index), VGA_SIZE);
}

uint8_t* kb_buf_() {
    return (uint8_t*) terminals[active_terminal_idx]._kb_buf;
}

uint32_t* kb_buf_top_() {
    return &terminals[active_terminal_idx]._kb_buf_top;
}

/* int32_t terminal_open();
 * Inputs: none
 * Return Value: 0
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
    if(buf_c == NULL){
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
        while(i < nbytes && *kb_buf_top <= kb_buf_size){
            buf_c[i] = kb_buf[i];
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
        memcpy((kb_buf_history[kb_buf_history_top]), kb_buf, kb_buf_size);
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
 * Return Value: 0
 */
int32_t terminal_close(int32_t fd){
    return 0;
}
