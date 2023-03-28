#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"

#define PASS 1
#define FAIL 0

// global variable to check test function output
int ret;

// set_atrib function takes in 4 bits for foreground color and 4 bits for background color
// colors we used are :
// 		- 0x07: black background with grey text
//		- 0x0B: black background with cyan text
//		- 0x02: black background with green text
//		- 0x4E: red background with yellow text
//      - 0x2f: green background with white text

/* format these macros as you see fit */
#define TEST_HEADER    \
    set_attrib(0x0B);\
    printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__);\
    set_attrib(0x07);
#define TEST_OUTPUT(name, result)    \
    printf("[TEST %s]\n", name); \
    ret = (result); \
    set_attrib(ret ? 0x02 : 0x04); \
    printf("%s\n", ret ? "PASS" : "FAIL");\
    set_attrib(0x07);

static inline void assertion_failure() {
    /* Use exception #15 for assertions, otherwise
       reserved by Intel */
    asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
    TEST_HEADER;

    int i;
    int result = PASS;
    for (i = 0; i < 10; ++i) {
        if ((idt[i].offset_15_00 == NULL) &&
            (idt[i].offset_31_16 == NULL)) {
            assertion_failure();
            result = FAIL;
        }
    }

    return result;
}

//add more tests here

/* Page Fault Exception Test
 *
 * Ensures dereferencing nullptr throws exception
 * Inputs: None
 * Outputs: None
 * Side Effects: Throws Page Fault Exception (Interrupt 14)
 * Coverage: Paging, Page Fault Exception
 * Files: paging.h/c, interrupt.h/c
 */
void page_fault_exception_test() {
    TEST_HEADER;
    int* p = NULL;
    int a;
    a = *p;
}

/* Page Overflow Test
 *
 * Ensures dereferencing out-of-bounds address throws exception
 * Inputs: None
 * Outputs: None
 * Side Effects: Throws Page Fault Exception (Interrupt 14)
 * Coverage: Paging, Page Fault Exception
 * Files: paging.h/c, interrupt.h/c
 */
void page_overflow_test() {
    TEST_HEADER;
    int* p = (int*) (0x800000 + 0x100000);
    int a;
    a = *p;
}

/* Paging VGA Test
 *
 * Ensures that the start of VGA memory is valid
 * Inputs: None
 * Outputs: PASS
 * Side Effects: Throws Page Fault Exception (Interrupt 14) if failure
 * Coverage: Paging
 * Files: paging.h/c
 */
int paging_vga_test() {
    TEST_HEADER;
    int* a = (int*) 0x000B8000; // Start of VGA memory
    int b;
    b = *a;
    return PASS;
}

/* Paging Kernel Test
 *
 * Ensures that the 12th address of kernel memory is valid
 * Completely arbitrary choice
 * Inputs: None
 * Outputs: PASS
 * Side Effects: Throws Page Fault Exception (Interrupt 14) if failure
 * Coverage: Paging
 * Files: paging.h/c
 */
int paging_kernel_test() {
    TEST_HEADER;
    int* a = (int*) ((1 << 22) + 12); // 12th address of kernel memory
    int b;
    b = *a;
    return PASS;
}

/* Paging Check Bounds
 *
 * Tests accessing all VGA and Kernel memory
 * Inputs: None
 * Outputs: PASS
 * Side Effects: Throws Page Fault Exception (Interrupt 14) if failure
 * Coverage: Paging
 * Files: paging.h/c
 */
int paging_check_bounds() {
    TEST_HEADER;
    char* a;
    char b;
    uint32_t i;
    printf("\t[*] Checking bounds of VGA text mode page - [0xB8000, 0xB8FFF]");
    for (i = 0xb8000; i < 0xb9000; i++) {
        a = (char*) i;
        b = *a;
    }
    set_attrib(0x02);
    printf(" ... success!\n");
    set_attrib(0x07);
    printf("\t[*] Checking bounds of Kernel page - [0x400000, 0x4FFFFF]");
    for (i = 0x400000; i < 0x500000; i++) {
        a = (char*) i;
        b = *a;
    }
    set_attrib(0x02);
    printf(" ... success!\n");
    set_attrib(0x2f);
    printf("\t[*] All valid memory addresses can be successfully accessed!\n");
    set_attrib(0x07);
    return PASS;
}

/* Paging Struct Test
 *
 * Tests that all elements of the structs have present==1
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Paging
 * Files: paging.h/c
 */
int paging_struct_test() {
    TEST_HEADER;
    int i;
    int flag = 1;
    for (i = 0; i < PAGE_DIC_MAX; i++) {
        flag = flag && page_directory[i].val;
    }
    for (i = 0; i < PAGE_TAB_MAX; i++) {
        flag = flag && page_table0[i].val;
    }
    return flag != 0 ? PASS : FAIL;
}

/* Divide Zero Test
 *
 * Performs division by 0
 * Inputs: None
 * Outputs: None
 * Side Effects: Raises Divide Error Exception (Interrupt 0)
 * Coverage: Divide Error Exception (Interrupt 0)
 * Files: interrupt.h/c
 */
void divide_zero_test() {
    int a = 0;
    int b = 0;
    a = a / b;
}

/* RTC Frequency Test
 *
 * Tests setting rtc frequencies as powers of 2 from 2-1024
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.h/c
 */
void rtc_freq_test() {
    unsigned i, j;
    clear();
    for (i = 2; i < 1025; i = i * 2) {
        printf("FREQUENCY IS %dHz\n", i);
        rtc_set_freq(i);
        for (j = 0; j < 600000000; j++);
        for (j = 0; j < 400000000; j++);
        printf("\n");
    }
    printf("RTC TEST DONE\n");
    rtc_set_rate(0);
    clear();
    reset_text_cursor();
}

/* Checkpoint 2 tests */
char terminal_flag[] = "391OS>";

void terminal_readwrite_test_Delay(){
    char buf [128];
    uint32_t fd = 0;
    terminal_open(3);
    int i;
    int ret;
    set_attrib(0x0b);
    printf("[*] Type ECE\\n and then 391\\n right away.\n[*] You should be able to see 391 after delay without pressing enter again.\n");
    set_attrib(0x07);
    if(TERMINAL_PROMPT_MODE)
        terminal_write(fd, (int8_t*)terminal_flag, (int32_t)TERMINAL_PROMPT_LEN);
    ret = terminal_read(fd, (int8_t*)buf, (int32_t)128);
    ret = terminal_write(fd,(int8_t*)buf, (int32_t)ret);
    for(i = 0; i < 600000000; i++);
    if(TERMINAL_PROMPT_MODE)
        terminal_write(fd, (int8_t*)terminal_flag, (uint32_t)TERMINAL_PROMPT_LEN);
    ret = terminal_read(fd, (int8_t*)buf, (int32_t)10);
    ret = terminal_write(fd, (int8_t*)buf, (int32_t)ret);
    terminal_close(3);
}
int terminal_open_close(){
    int32_t fd = 0;
    if(terminal_close(fd) == -1 && terminal_open(fd) == -1){
        return PASS;
    }
    return FAIL;
}

int terminal_write_test(){
    int8_t buf [200];
    uint32_t fd = 0;
    int i;
    int ret;
    for(i = 0; i < 200; i++){
        buf[i] = '0';
    }
    ret = terminal_write(fd, buf, 200);
    for(i = 0; i < 600000000; i++);
    for(i = 0; i < 200; i++){
        buf[i] = '\b';
    }
    ret = terminal_write(fd, buf, 200);
    for(i = 0; i < 600000000; i++);
    set_attrib(0x0b);
    printf("\nWRITE %d bytes\n", ret);
    printf("[*] If you don't see any `0`s on the terminal, you passed backspace test also.\n");
    set_attrib(0x07);
    if(ret != 200){
        return FAIL;
    }
    return PASS;
}

int terminal_read_of(){
    int8_t buf [500];
    int32_t fd = 0;
    int ret;
    set_attrib(0x0b);
    printf("[*] Type as much as possible\n");
    set_attrib(0x07);
    ret = terminal_read(fd, buf, 500);
    set_attrib(0x0b);
    printf("[*] READ %d bytes\n", ret);
    set_attrib(0x07);
    if(ret != 128){
        return FAIL;
    }
    return PASS;
}

void terminal_readwrite_test_inf(){
    char buf [128];
    int ret;
    int32_t fd = 0;
    set_attrib(0x0b);
    printf("[*] Sandbox testing: Type `exit` to leave this test\n");
    set_attrib(0x07);
    char exit_flag[] = "exit\n";
    terminal_open(3);
    while(1){
        if(TERMINAL_PROMPT_MODE)
            terminal_write(fd, (int8_t*)terminal_flag, (int32_t)TERMINAL_PROMPT_LEN);
        ret = terminal_read(fd, (int8_t*)buf, (int32_t)128);
        set_attrib(0x0B);
        printf("[*] READ %d bytes\n", ret);
        set_attrib(0x07);
        ret = terminal_write(fd, (int8_t*)buf, (int32_t)ret);
        set_attrib(0x0B);
        printf("[*] WRITE %d bytes\n", ret);
        set_attrib(0x07);
        if(strncmp(buf, exit_flag, 4) == 0){
            break;
        }
    }
    terminal_close(fd);
}


int formatted_string_test(){
    int32_t fd = 0;
    int ret;
    int8_t str[] = "Hello\n\n\tWorld!!\b\n";
    int len = strlen(str);
    ret = terminal_write(fd, str, len);
    printf("If you see:\nHello\n\n\tWorld!\nThen you passed this test\n");
    if(ret == len){
        return PASS;
    }
    return FAIL;

}

int fish_string_test(){
    int32_t fd = 0;
    int ret;
    char* str =      "/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n"
                     "         o\n"
                     "           o    o\n"
                     "       o\n"
                     "             o\n"
                     "        o     O\n"
                     "    _    \\\n"
                     " |\\/.\\   | \\/  /  /\n"
                     " |=  _>   \\|   \\ /\n"
                     " |/\\_/    |/   |/\n"
                     "----------M----M--------\n";
    int len = strlen(str);
    ret = terminal_write(fd, str, len);
    printf("If you see fish then you passed this test :D\n");
    set_attrib(0x0B);
    printf("[*] WRITE %d bytes\n", ret);
    set_attrib(0x07);
    if(ret == len){
        return PASS;
    }
    return FAIL;

}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests() {
    int i;
    clear();
    reset_text_cursor();
    // launch your tests here
    // idt test
    TEST_OUTPUT("idt_test", idt_test());

    // rtc changing frequency test
    //rtc_freq_test();

    // paging tests
    TEST_OUTPUT("paging_struct_test", paging_struct_test());
    TEST_OUTPUT("paging_vga_test", paging_vga_test());
    TEST_OUTPUT("paging_kernel_test", paging_kernel_test());
    TEST_OUTPUT("paging upper and lower bound test", paging_check_bounds());
//    page_overflow_test();
//    page_fault_exception_test();

    // zero-division exception
//    divide_zero_test();
    //terminal_readwrite_test();
    TEST_OUTPUT("Terminal open and close returns -1", terminal_open_close());
    TEST_OUTPUT("Terminal overflow: read", terminal_read_of());
    TEST_OUTPUT("Terminal test: write", terminal_write_test());
    TEST_OUTPUT("Formatted string", formatted_string_test());
    for(i = 0; i < 600000000; i++);
    for(i = 0; i < 600000000; i++);
    TEST_OUTPUT("Formatted string: fish", fish_string_test());
    terminal_readwrite_test_Delay();
    terminal_readwrite_test_inf();
}

