#include "terminal.h"

#define TERMINAL_TEXT_COLOR 0x3C
#define TERMINAL_BACKGROUND_COLOR 0x03
#define TERMINAL_NUM 3
#define TERMINAL_INPUT_BUF_SIZE 128
#define TERMINAL_TEXT_BUF_SIZE 2100



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
    if(kb_buf_top == 0 &&(up_flag == 1 && kb_buf_history_top > 0 && kb_buf_history_ptr > 0)){
        // if up was pressed in the top of keyboard history, we copy
        // what we have so far into the most recent history and save num chars
        if(kb_buf_history_ptr == kb_buf_history_top){
            memcpy(kb_buf_history[kb_buf_history_top], kb_buf, kb_buf_size);
            kb_buf_top_cached = kb_buf_top;
        }
        // if current terminal argument is longer than screen width, delete stuff
        // so that cursor is in the first line
        if(kb_buf_top > screen_w - 1){
            while(kb_buf_top > screen_w - 1){
                putc('\b');
                pop_kb_buf();
            }
        }
        // point at next most recent history
        kb_buf_history_ptr--;
        // copy contents of that history buffer to keyboard buffer
        memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], kb_buf_size);
        i = 0;
        // print the saved buffer to screen, after clearing current line.
        clear_line();
        //terminal_write(1, (void*)"391OS> ",7);
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
        // if we are printing the in-progress buffer, there is no newline char
        // to terminate on. Use cached buffer length as a terminator.
        if(kb_buf_history_ptr == kb_buf_history_top){
            i = 0;
            clear_line();
            // terminal_write(1, (void*)"391OS> ",7);
            memcpy(kb_buf, kb_buf_history[kb_buf_history_top], kb_buf_size);
            kb_buf_top = kb_buf_top_cached;
            while(i < kb_buf_top_cached){
                putc(kb_buf[i]);
                i++;
            }
        }
        else{
            // for other saved buffers you just print until newline after clearing line.
            // same thing as above.
            memcpy(kb_buf, kb_buf_history[kb_buf_history_ptr], kb_buf_size);
            i = 0;
            clear_line();
            // terminal_write(1, (void*)"391OS> ",7);
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
    kb_buf_history_ptr = 0; //the history function need to be transfer to terminal structure
    kb_buf_history_top = 0;
#if (muti_terminal==1)
    int i;
    for(i=0; i<3; i++){
        terminal_init_each(&terminal_arr[i], text_bufs[i], input_bufs[i]);
    }
#endif
}

#if (muti_terminal==1)
int terminal_init_each(terminal* terminal, char* text_buf, char* input_buf){
    terminal->start_row_index = 0;
    terminal->cur_x = 0;
    terminal->cur_y = 0;
    terminal->text_color = TERMINAL_TEXT_COLOR;
    terminal->background_color = TERMINAL_BACKGROUND_COLOR;

    terminal->text_buf = text_buf;
    terminal->input_buf_cur_pos = 0;
    terminal->input_buf = input_buf;

    return 0;
}
#endif

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
        while(i < nbytes && kb_buf_top <= kb_buf_size){
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
 * Return Value: -1
 * Function: terminal shouldn't be closed for cp2.
 */
int32_t terminal_close(int32_t fd){
    return 0;
}

#if (muti_terminal==1)
void terminal_buf_put(terminal* terminal, char c){
    if (terminal->input_buf_cur_pos < TERMINAL_INPUT_BUF_SIZE ){
        terminal->input_buf[terminal->input_buf_cur_pos] = c;
        terminal->input_buf_cur_pos++;
    }
    return;
}

void terminal_buf_delete(terminal* terminal){
    if (terminal->input_buf_cur_pos > 0){
        terminal->input_buf_cur_pos--;
        return;
    }
    return;
}

void terminal_buf_clear(terminal* terminal){
    terminal->input_buf_cur_pos = 0;
    return;
}


terminal* terminal_alloc(){
    int i;
    int index;
    if (terminal_used[TERMINAL_NUM-1]==1) return NULL;
    
    for(i=0; i<TERMINAL_NUM; i++){
        
        if(terminal_used[i]==0){
            terminal_used[i] = 1;
            index= i;
        }
        
    }
    terminal* terminal = &terminal_arr[index];
    return terminal;
}

int terminal_free(terminal* terminal){
    int index = ((uint32_t)terminal - (uint32_t)terminal_arr) / sizeof(terminal_struct);
    if(index<0 || index>=TERMINAL_NUM) return -1;
    terminal_used[index] = 0;
    return 0;
}

terminal* terminal_get(int index){
    if(index<0 || index>=TERMINAL_NUM) return NULL;
    return &terminal_arr[index];
}

int terminal_get_index(terminal* terminal){
    int index = ((uint32_t)terminal - (uint32_t)terminal_arr) / sizeof(terminal_struct);
    if(index<0 || index>=TERMINAL_NUM) return -1;
    return index;
}
#endif
