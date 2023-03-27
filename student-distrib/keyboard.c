#include "keyboard.h"
void keyboard_init(){
    int i;
    enable_irq(0x1);
    for(i = 0; i < 128; i++){
        kb_buf[i] = 0;
    }
    kb_buf_top = 0;
    caps_flag = 0, shift_flag = 0, alt_flag = 0, ctrl_flag = 0, enter_flag = 0;
}

int is_printable(uint8_t ascii){
    if((ascii > 0x19 && ascii < 0x7D) || ascii == '\t' || ascii == '\b' || ascii == '\n' || ascii == '\r')
        return 1;
    else
        return 0;
}

// only changes alphabetic keys who are also affected by capslock
int is_capsable(uint8_t ascii){
    if(ascii > 0x60 && ascii < 0x7B)
        return 1;
    else
        return 0;
}

int convert_case(uint8_t ascii){
    uint8_t ret = ascii;
    if ((caps_flag) && is_capsable(ascii)){
        ret = UPPERCASE(ascii);
    }
    else if(shift_flag){
        // for alphabetic characters and some other
        if(ascii > 0x60 && ascii < 0x7E)
            ret = UPPERCASE(ascii);
        // for numbers
        else{
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

void push_kb_buf(uint8_t ascii){
    kb_buf[kb_buf_top] = ascii;
    kb_buf_top++;
}

uint8_t pop_kb_buf(){
    if(kb_buf_top > 0){
        kb_buf_top--;
        return kb_buf[kb_buf_top];
    }
    return NULL;
}

void clear_kb_buf(){
    int i;
    for(i = 0; i < 128; i++){
        kb_buf[i] = 0;
    }
    kb_buf_top = 0;
}

void print_history(){
    int i, j, ctr;
    ctr = 0;
    set_attrib(0x0b);
    printf("Terminal History (5 most recent):\n");
    for (i = kb_buf_history_top - 1; 0 <= i && ctr < 5; i--, ctr++){
        j = 0;
        printf("\t[*] ");
        while(1){
            putc(kb_buf_history[i][j]);
            if(kb_buf_history[i][j] == '\n' || kb_buf_history[i][j] == '\r') break;
            j++;
        }
    }
    set_attrib(0x07);
}

void print_history_full(){
    int i, j;
    set_attrib(0x0b);
    printf("Terminal History (Full):\n");
    for (i = kb_buf_history_top - 1; 0 <= i; i--){
        j = 0;
        printf("\t[*] ");
        while(1){
            putc(kb_buf_history[i][j]);
            if(kb_buf_history[i][j] == '\n' || kb_buf_history[i][j] == '\r') break;
            j++;
        }
    }
    set_attrib(0x07);
}

void keyboard_handler(){
    int i;
    uint8_t scancode;
    uint8_t ascii;
    scancode = inb(0x60);
    set_special_flags(scancode);
    ascii = scancode_down[scancode];
    if(scancode < DOWN_SIZE){
        if(is_printable(ascii)){
            ascii = convert_case(ascii);
            if(!(alt_flag || ctrl_flag)){
                if(ascii == '\b'){
                    uint8_t tmp = pop_kb_buf();
                    if(tmp == '\t'){
                        for(i = 0; i < 4; i++){
                            putc(ascii);
                        }
                    }
                    else{
                        putc(ascii);
                    }
                }
                else{
                    if(ascii == '\n' || ascii == '\r'){
                        enter_flag = 1;
                    }
                    if(kb_buf_top < 127){
                        push_kb_buf(ascii);
                        putc(ascii);
                    }
                    if(kb_buf_top == 127 && (ascii == '\n' || ascii == '\r')){
                        push_kb_buf(ascii);
                        putc(ascii);
                    }
                }
            }
            if(ctrl_flag && (ascii == 'l' || ascii == 'L')){
                clear();
                reset_text_cursor();
            }
            if(ctrl_flag && (ascii == 'h')){
                if(kb_buf_top==0)
                    print_history();
            }
            if(ctrl_flag && (ascii == 'H')){
                if(kb_buf_top==0)
                    print_history_full();
            }
        }
    }
    // handling arrow keys
    if(scancode == DOWN || scancode == UP || scancode == RIGHT || scancode == LEFT){
        handle_arrowkeys(scancode);
    }
    send_eoi(0x1);
}
