#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 200

#define BLACK 0
#define WHITE 0x3F
#define RED 0x30

#define BLACK_HEIGHT 97
#define WHITE_HEIGHT 147
#define BLACK_WIDTH 11
#define WHITE_WIDTH 22

static uint32_t key_to_freq[24] = {262, 277, 294, 311, 329, 349, 370, 392, 415, 440, 466, 494, 
                                    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 987};

int main(){
    return -1;
}