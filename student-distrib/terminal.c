#include "terminal.h"

char* vga_mem = (char*)VGA_MEMORY;

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

int terminal_init_each(terminal_t* terminal){
    terminal->start_row_index = 0;
    terminal->cur_x = 0;
    terminal->cur_y = 0;
    terminal->text_color = 0x3C;
    return 0;
}

/* void terminal_init();
 * Inputs: none;
 * Return Value: none
 * Function: initializes terminal history stack pointer and index to 0.
 */
void terminal_init(){
    kb_buf_history_ptr = 0;
    kb_buf_history_top = 0;

    curr_term_sched = -1;
    curr_term_displayed = 0;
    int i;
    for(i=0; i<3; i++){
        terminal_init_each(&terminal_arr[i]);
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
            terminal_t* terminal = &terminal_arr[curr_term_displayed];
            multi_terminal_putc(terminal,*((int8_t*)buf+i));
            //xy_get(terminal->cur_x,terminal->cur_y);
            //printf("cur_x is %d,cur_y is %d\n",terminal->cur_x,terminal->cur_y);
            //putc(*((int8_t*)buf+i));
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

#if TEST_MUL

static int text_mem_array[3] = {VGA_TEXT_BUF_ADDR, VGA_TEXT_BUF_ADDR1,  VGA_TEXT_BUF_ADDR2};
static int video_mem_array_show[3] = {0, (VGA_TEXT_BUF_ADDR1 - VGA_TEXT_BUF_ADDR)/2,  (VGA_TEXT_BUF_ADDR2 - VGA_TEXT_BUF_ADDR)/2};

terminal_t* terminal_get(int index){
    if(index<0 || index>=TERMINAL_NUM) return NULL;
    return &terminal_arr[index];
}

int terminal_get_index(terminal_t* terminal){
    int index = ((uint32_t)terminal - (uint32_t)terminal_arr) / sizeof(terminal_t);
    if(index<0 || index>=TERMINAL_NUM) return -1;
    return index;
}

/*
shift VGA screen up
input:none
output:1 for success
side effect:change video memory
*/
inline int vga_up_shift(){
    unsigned int x,y;
    unsigned int index, index2;
    for(y=0;y<25  -1;y++){
        for(x=0;x<80 ;x++){
            index = y * 80  + x;
            index2 = (y + 1) *80  + x;
            *(uint8_t *)(vga_mem + (index << 1)) = *(uint8_t *)(vga_mem + (index2 << 1));
            *(uint8_t *)(vga_mem + (index << 1) + 1) = *(uint8_t *)(vga_mem + (index2 << 1) + 1);
        }
    }
    for(x=0;x<80 ;x++){
        index = y * 80  + x;
        *(uint8_t *)(vga_mem + (index << 1)) = ' ';
    }
    return 1;
}

inline int vga_print(int x, int y, char c, unsigned char color){
    uint32_t index = y * 80 + x;
    *(uint8_t *)(vga_mem + (index << 1)) = c;
    *(uint8_t *)(vga_mem + (index << 1) + 1) = color;
    return 1;
}

int vga_set_cursor(int index, int x, int y){
    unsigned short pos = video_mem_array_show[index] + y * screen_w + x;
    //if(top_pcb==NULL || index != terminal_get_index(top_pcb->terminal)) return 0;
	outb(14, VGA_CMD); // write to 14th registers
    // 0x3d4 is command port
	outb((pos >> 8) & 0xff, VGA_DATA);
    // 0x3d5 is data port
	outb(15, VGA_CMD);  // write to 15th registers
	outb(pos & 0xff, VGA_DATA);
    return 1;
}

void vga_show_set(int index){
    if (index<0 || index > 2){
        return;
    }
    uint16_t address = (uint16_t)video_mem_array_show[index];
    outb(0x0c, VGA_CMD);
    outb(((address)>>8), VGA_DATA);
    // 0x0c holds higher video memory address
    // takes higher 4 bits
    outb(0x0d, VGA_CMD);
    outb(((address)&0x00ff), VGA_DATA);
    // 0x0d holds lower video memory address
    // takes lower 4 bits
    terminal_t* terminal = terminal_get(index);
    vga_set_cursor(index, terminal->cur_x, terminal->cur_y);
    return;
}

void vga_write_set(int index){
    if (index<0 || index > 2){
        return;
    }
    vga_mem = (char*)text_mem_array[index];
    return;
}

int multi_terminal_putc(terminal_t* terminal, char c){

    uint32_t flags;
    cli_and_save(flags);
    if (terminal == NULL){
        return -1;
    }
    int index = terminal_get_index(terminal);
    vga_write_set(index);
    if(c == '\n' || c == '\r'){
        terminal->cur_x = 0;
        if(terminal->cur_y + 1 ==  25){
            vga_up_shift();
        }else{
            terminal->cur_y++;
        }
    }else{
        vga_print(terminal->cur_x, terminal->cur_y, c, terminal->text_color);
        // also record the text buf data
        //terminal->text_buf[terminal->cur_x + terminal->cur_y * 80] = c;
        // store the text at text buffer
        if(terminal->cur_x + 1 == 80 ){
            if(terminal->cur_y + 1 == 25){
                terminal->cur_x = 0;
                terminal->start_row_index = (terminal->start_row_index + 1) % 25;
                vga_up_shift();
            }else{
                terminal->cur_y++;
                terminal->cur_x = 0;
            }
        }else{
            terminal->cur_x++;
        }
    }
    vga_set_cursor(index, terminal->cur_x, terminal->cur_y);
    restore_flags(flags);
    return 1;
}

#endif
