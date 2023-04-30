#include "keyboard.h"

#define ASCII_SPACE 0x20
#define ASCII_TILDA 0x7E
#define ASCII_a 0x61
#define ASCII_z 0x7a

// set_atrib function takes in 4 bits for foreground color and 4 bits for background color
// colors we used are :
// 		- 0x07: black background with grey text
//		- 0x0B: black background with cyan text
//		- 0x02: black background with green text
//		- 0x4E: red background with yellow text
//      - 0x2f: green background with white text

/* void keyboard_init();
 * Inputs: none
 * Return Value: none
 * Function: enables keyboard interrupt, flushes keyboard buffer, and initializes special
 *           key flags to 0.
 */
void keyboard_init(){
    int i;
    enable_irq(0x1);
    // set keyboard buffer to 0
    for(i = 0; i < kb_buf_size; i++){
        kb_buf[i] = 0;
    }
    // reinitialize various variables
    kb_buf_top = 0;
    caps_flag = 0, shift_flag = 0, alt_flag = 0, ctrl_flag = 0, enter_flag = 0;
}

/* int keyboard_init(uint8_t ascii);
 * Inputs: uint8_t ascii
 * Return Value: 1 if key is a printable key 0 if not
 * Function: looks at the ascii code input and determines if this should be 
 * printed on screen or not. Includes special characters
 */
int is_printable(uint8_t ascii){
    if((ascii >= ASCII_SPACE && ascii <= ASCII_TILDA) || ascii == '\t' || ascii == '\b' || ascii == '\n' || ascii == '\r')
        return 1;
    else
        return 0;
}

/* int is_capsable(uint8_t ascii);
 * Inputs: uint8_t ascii
 * Return Value: 1 if key should be changed to uppercase when capslock is enabled, 0 if not
 * Function: looks at the ascii code input and determines if this should be 
 *           changed to upper case when caps lock is enabled. Basically checks if it is alphabet letter
 */
int is_capsable(uint8_t ascii){
    if(ascii >= ASCII_a && ascii <= ASCII_z)
        return 1;
    else
        return 0;
}

/* int convert_case(uint8_t ascii);
 * Inputs: uint8_t ascii
 * Return Value: ascii value of upper case version of input character, if applicable.
 * Function: looks at the ascii code input and determines changes lower case letter to upper case letter.
 *           also changes number keys into special characters
 */
int convert_case(uint8_t ascii){
    uint8_t ret = ascii;
    if ((caps_flag) && is_capsable(ascii) && !shift_flag){
        ret = UPPERCASE(ascii);
    }
    else if(shift_flag){
        // for alphabetic characters and some other
        // uppercase state of alphabetic keys are set by both shift and capslock -- mutually exclusive
        if(ascii >= ASCII_a && ascii <= ASCII_z && !caps_flag)
            ret = UPPERCASE(ascii);
        // for numbers
        else{
            // switch cases for number keys
            switch(ascii){
                case '0':
                    ret = ')';
                    break;
                case '1':
                    ret = '!';
                    break;
                case '2':
                    ret = '@';
                    break;
                case '3':
                    ret = '#';
                    break;
                case '4':
                    ret = '$';
                    break;                
                case '5':
                    ret = '%';
                    break;                
                case '6':
                    ret = '^';
                    break;                
                case '7':
                    ret = '&';
                    break;                
                case '8':
                    ret = '*';
                    break;                
                case '9':
                    ret = '(';
                    break;                
                case ',':
                    ret = '<';
                    break;                
                case '.':
                    ret = '>';
                    break;                
                case '/':
                    ret = '?';
                    break;
                case '[':
                    ret = '{';
                    break;
                case ']':
                    ret = '}';
                    break;
                case '\\':
                    ret = '|';
                    break;
                case '-':
                    ret = '_';
                    break;
                case '=':
                    ret = '+';
                    break;
                case '`':
                    ret = '~';
                    break;
                default : break;
            }                
        }
    }
    return ret;
}

#define DOWN_SIZE 0x54
// Conversion of scancodes to ASCII characters. Only non-shift version
char scancode_down [DOWN_SIZE] = {
    0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, // 0x00 - 0x07
    0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09, // 0x08 - 0x0f
    0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, // 0x10 - 0x17
    0x6f, 0x70, 0x5b, 0x5d, 0x0d, 0x00, 0x61, 0x73, // 0x18 - 0x1f
    0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, // 0x20 - 0x27
    0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76, // 0x28 - 0x2f
    0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A, // 0x30 - 0x37
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x38 - 0x3f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x40 - 0x47
    0x00, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x2B, 0x00, // 0x48 - 0x4f
    0x00, 0x00, 0x00, 0x00// 0x50 - 0x53

};

/* void set_special_flags(uint8_t scancode);
 * Inputs: uint8_t scancode -- scancode from keyboard
 * Return Value: none.
 * Function: sets special key's flag variables if applicable.
 */
void set_special_flags(uint8_t scancode){
    switch(scancode){
        case LSHIFT:
            shift_flag = 1;
            break;
        case KEY_UP(LSHIFT):
            shift_flag = 0;
            break;
        case RSHIFT:
            shift_flag = 1;
            break;
        case KEY_UP(RSHIFT):
            shift_flag = 0;
            break;
        case CAPS:
            caps_flag ^= 1;
            break;
        case  CTRL:
            ctrl_flag = 1;
            break;
        case KEY_UP(CTRL):
            ctrl_flag = 0;
            break;
        case ALT:
            alt_flag = 1;
            break;
        case KEY_UP(ALT):
            alt_flag = 0;
            break;
        // case ENTER: // enter flag is unset once our handlers are done with their job. maybe the name should be newline_flag
        //     enter_flag = 1;
        //     break;
        // case KEY_UP(ENTER):
        //     enter_flag = 0;
        //     break;
        default: break;
    }
}

/* void push_kb_buf(uint8_t ascii);
 * Inputs: uint8_t ascii
 * Return Value: none.
 * Function: pushes the new ascii input character to keyboard buffer and update the
 *           top of stack variable
 */
void push_kb_buf(uint8_t ascii){
    kb_buf[kb_buf_top] = ascii;
    kb_buf_top++;
}


/* uint8_t pop_kb_buf(uint8_t ascii);
 * Inputs: none
 * Return Value: ascii value of popped character.
 * Function: popps the newest added ascii input character from keyboard buffer and update the
 *           top of stack variable
 */
uint8_t pop_kb_buf(){
    // remove most recent push and return that. if there is underflow, return null.
    if(kb_buf_top > 0){
        kb_buf_top--;
        return kb_buf[kb_buf_top];
    }
    return NULL;
}

/* void clear_kb_buf()
 * Inputs: none
 * Return Value: none
 * Function: resets keyboard buffer to null characters.
 */
void clear_kb_buf(){
    int i;
    for(i = 0; i < kb_buf_size; i++){
        kb_buf[i] = 0;
    }
    kb_buf_top = 0;
}
#if ENABLE_HISTORY
#define history_depth 5

/* void print_history()
 * Inputs: none
 * Return Value: none
 * Function: Grabs "history_depth" most recent terminal commands sent and prints to screen.
 */
void print_history(){
    int i, j, ctr;
    // variable for history depth
    ctr = 0;
    // set text to cyan with black bg
    set_attrib(0x0b);
    clear_line();
    printf("Terminal History (%d most recent):\n", history_depth);
    for (i = kb_buf_history_top - 1; 0 <= i && ctr < history_depth; i--, ctr++){
        j = 0;
        printf("\t[*] ");
        // print characters right until there is a newline character
        while(1){
            putc(kb_buf_history[i][j]);
            if(kb_buf_history[i][j] == '\n' || kb_buf_history[i][j] == '\r') break;
            j++;
        }
    }
    // restore text color
    set_attrib(0x07);
    terminal_write(1, (void*)"391OS> ",7);
}

/* void print_history()
 * Inputs: none
 * Return Value: none
 * Function: Grabs all previous terminal commands sent and prints to screen.
 */
void print_history_full(){
    int i, j;
    // set text to cyan with black bg
    set_attrib(0x0b);
    printf("Terminal History (Full):\n");
    for (i = kb_buf_history_top - 1; 0 <= i; i--){
        j = 0;
        printf("\t[*] ");
        // print characters right until there is a newline character
        while(1){
            putc(kb_buf_history[i][j]);
            if(kb_buf_history[i][j] == '\n' || kb_buf_history[i][j] == '\r') break;
            j++;
        }
    }
    // restore text graphics
    set_attrib(0x07);
}
#endif

#define KB_SCANCODE_PORT 0x60
#define F1_DOWN 0x3B
#define F3_DOWN 0x3D


/* void keyboard_handler()
 * Inputs: none
 * Return Value: none
 * Function: Keyboard interrupt handler that rules them all.
 */
void keyboard_handler(){
    int i;
    uint8_t scancode;
    uint8_t ascii;
    // grab scancode from keyboard
    scancode = inb(KB_SCANCODE_PORT);
    set_special_flags(scancode);
    ascii = scancode_down[scancode];

    // if scancode is a printable code, print on screen!
    if(scancode < DOWN_SIZE){
        if(is_printable(ascii)){
            ascii = convert_case(ascii);
            if(!(alt_flag || ctrl_flag)){
                // if backspace we need to pop that from keyboard buffer
                if(ascii == '\b'){
                    if(kb_buf_top > 0){
                        uint8_t tmp = pop_kb_buf();
                        // if the popped key is tab key, we need to delete 4 characters
                        if(tmp == '\t'){
                            for(i = 0; i < TAB_SIZE; i++){
                                terminal_t* terminal = &terminal_arr[curr_term_displayed];
                                multi_terminal_putc(terminal,ascii);
                                //putc(ascii);
                            }
                        }
                        else{
                            terminal_t* terminal = &terminal_arr[curr_term_displayed];
                            multi_terminal_putc(terminal,ascii);
                            //putc(ascii);
                        }
                    }
                }
                else{
                    if(ascii == '\n' || ascii == '\r'){
                        enter_flag = 1;
                    }
                    // only print to screen and pus to keyboard buffer up til 127th character
                    if(kb_buf_top < kb_buf_size - 1){
                        push_kb_buf(ascii);
                        terminal_t* terminal = &terminal_arr[curr_term_displayed];
                        multi_terminal_putc(terminal,ascii);
                        //putc(ascii);
                    }
                    // 128th character is reserved for newline character
                    if(kb_buf_top == kb_buf_size - 1 && (ascii == '\n' || ascii == '\r')){
                        push_kb_buf(ascii);
                        terminal_t* terminal = &terminal_arr[curr_term_displayed];
                        multi_terminal_putc(terminal,ascii);
                        //putc(ascii);
                    }
                }
            }
            // ctrl-l is clearing screen and reset cursor location.
            if(ctrl_flag && (ascii == 'l' || ascii == 'L')){
                clear();
                reset_text_cursor();
            }
            // ctrl-c halts current program
            if(ctrl_flag && (ascii == 'c' || ascii == 'C')){
                printf("\n");
                set_attrib(0x4E);
                printf("[!] Program received signal SIGINT. Keyboard Interrupt.");
                set_attrib(0x7);
                printf("\n");
                send_eoi(0x1);
                syscall_halt(0);
            }
            #if ENABLE_HISTORY
            // ctrl-h prints user-set number of history
            if(ctrl_flag && (ascii == 'h')){
                if(kb_buf_top==0)
                    print_history();
            }
            // ctrl-H prints all history
            if(ctrl_flag && (ascii == 'H')){
                if(kb_buf_top==0)
                    print_history_full();
            }
            #endif
        }
    }
    // handling alt-f_ key for terminal switching
    if(alt_flag == 1){
        if(scancode >= F1_DOWN && scancode <= F3_DOWN){
            uint32_t terminal_idx = scancode - F1_DOWN;
            terminal_switch(terminal_idx);
        }
    }

    // handling arrow keys
    if(scancode == DOWN || scancode == UP || scancode == RIGHT || scancode == LEFT){
        handle_arrowkeys(scancode);
    }
    send_eoi(0x1);
}
