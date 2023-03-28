#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"

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
    printf("[TEST] Running %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__);\
    set_attrib(0x07);
#define TEST_OUTPUT(name, result)    \
    ret = (result); \
    set_attrib(ret ? 0x02 : 0x04); \
    printf("%s\n", ret ? "PASS" : "FAIL");\
    set_attrib(0x07);
#define PRINT_PASS \
    set_attrib(0x02); \
    printf("PASS\n"); \
    set_attrib(0x07);

static inline void assertion_failure() {
  /* Use exception #15 for assertions, otherwise
     reserved by Intel */
  asm volatile("int $15");
}

/* add_delay
 *
 * helper function to add delay
 * Inputs: None
 * Outputs: None
 * Side Effects: adds delay
 */
void add_delay(){
    int i = 0;
    for (i = 0; i < 600000000; i++);
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

/* RTC Frequency Bounds Test
 *
 * Testing different RTC frequencies to check if inbounds of 2-1024 and power of 2
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.h/c
 */
int rtc_freq_bounds_test() {
    TEST_HEADER;
    unsigned buf = 0;
    int32_t ret;
    ret = rtc_write(NULL, &buf, NULL);
    buf = -1;
    ret = rtc_write(NULL, &buf, NULL);
    buf = 1;
    ret = rtc_write(NULL, &buf, NULL);
    buf = 11;
    ret = rtc_write(NULL, &buf, NULL);
    buf = 1025;
    ret = rtc_write(NULL, &buf, NULL);

    if (ret == -1) {
        return PASS;
    } else {
        return FAIL;
    }
}

/* RTC Frequency open, read, write, close functions Test
 *
 * Testing all of the open, read, write, and close RTC functions
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.h/c
 */
int rtc_functions_test() {
  TEST_HEADER;
  int i, j;
  unsigned freq = 1;
  rtc_open(NULL); //freq set to 2hz
  printf("[*] Number of interrupts has ceil of 480 (6 lines)");
  for (i = 0; i < 10; i++) { //increase frequency by power of 2 up to 1024(max)
    freq *= 2;
    printf("\n[*] FREQUENCY SET TO %d\n", freq);
    rtc_write(NULL, &freq, NULL);   // Write in new freq value
    for (j = 0; j < freq * 2 && j < 480; j++) { // wait for freq/2 interrupts before updating
      rtc_read(NULL, NULL, NULL); //read waits until next interrupt
    }
  }
  // disable rtc for later purposes
  rtc_set_freq(0);
  rtc_close(NULL);
  return PASS;
};


char terminal_flag[] = "391OS>";

/* Terminal readwrite test - delay
 *
 * Tests the behavior of two reads when there is delay between the two.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
void terminal_readwrite_test_Delay() {
  TEST_HEADER;
  char buf[128];
  uint32_t fd = 0;
  terminal_open(3);
  int i;
  int ret;
  set_attrib(0x0b);
  printf(
      "[*] Type ECE\\n and then 391\\n right away.\n[*] You should be able to see 391 after delay without pressing enter again.\n");
  set_attrib(0x07);
  if (TERMINAL_PROMPT_MODE)
    terminal_write(fd, (int8_t*) terminal_flag, (int32_t) TERMINAL_PROMPT_LEN);
  ret = terminal_read(fd, (int8_t*) buf, (int32_t) 128);
  ret = terminal_write(fd, (int8_t*) buf, (int32_t) ret);
  for (i = 0; i < 600000000; i++);
  if (TERMINAL_PROMPT_MODE)
    terminal_write(fd, (int8_t*) terminal_flag, (uint32_t) TERMINAL_PROMPT_LEN);
  ret = terminal_read(fd, (int8_t*) buf, (int32_t) 10);
  ret = terminal_write(fd, (int8_t*) buf, (int32_t) ret);
  terminal_close(3);
}

/* Terminal_open_close test
 *
 * Tests the behavior of open and close of terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int terminal_open_close() {
  TEST_HEADER;
  int32_t fd = 0;
  if (terminal_close(fd) == -1 && terminal_open(fd) == -1) {
    return PASS;
  }
  return FAIL;
}

/* terminal_write_test
 *
 * Tests the behavior of writing characters bigger than 128 bytes and backspaces
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int terminal_write_test() {
  TEST_HEADER;
  int8_t buf[200];
  uint32_t fd = 0;
  int i;
  int ret;
  for (i = 0; i < 200; i++) {
    buf[i] = '0';
  }
  ret = terminal_write(fd, buf, 200);
  for (i = 0; i < 600000000; i++);
  for (i = 0; i < 200; i++) {
    buf[i] = '\b';
  }
  ret = terminal_write(fd, buf, 200);
  for (i = 0; i < 600000000; i++);
  set_attrib(0x0b);
  printf("\nWRITE %d bytes\n", ret);
  printf("[*] If you don't see any `0`s on the terminal, you passed backspace test also.\n");
  set_attrib(0x07);
  if (ret != 200) {
    return FAIL;
  }
  return PASS;
}

/* terminal_read_of
 *
 * Tests the behavior of reading keyboard buffer where nbytes is greater than the size of kbd buf
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int terminal_read_of() {
  TEST_HEADER;
  // here, 500 is some arbitrary number that is greater than 128 (keyboard buffer size)
  int8_t buf[500];
  int32_t fd = 0;
  int ret;
  set_attrib(0x0b);
  printf("[*] Type as much as possible\n");
  set_attrib(0x07);
  ret = terminal_read(fd, buf, 500);
  set_attrib(0x0b);
  printf("[*] READ %d bytes\n", ret);
  set_attrib(0x07);
  if (ret != 128) {
    return FAIL;
  }
  return PASS;
}

/* terminal_readwrite_test_inf
 *
 * Generic test that echos terminal input to screen
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
void terminal_readwrite_test_inf() {
  TEST_HEADER;
  char buf[kb_buf_size];
  int ret;
  int32_t fd = 0;
  set_attrib(0x0b);
  printf("[*] Sandbox testing: Type `exit` to leave this test\n");
  set_attrib(0x07);
  char exit_flag[] = "exit\n";
  terminal_open(3);
  while (1) {
    if (TERMINAL_PROMPT_MODE)
      terminal_write(fd, (int8_t*) terminal_flag, (int32_t) TERMINAL_PROMPT_LEN);
    ret = terminal_read(fd, (int8_t*) buf, (int32_t) kb_buf_size);
    set_attrib(0x0B);
    printf("[*] READ %d bytes\n", ret);
    set_attrib(0x07);
    ret = terminal_write(fd, (int8_t*) buf, (int32_t) ret);
    set_attrib(0x0B);
    printf("[*] WRITE %d bytes\n", ret);
    set_attrib(0x07);
    if (strncmp(buf, exit_flag, 4) == 0) {
      break;
    }
  }
  terminal_close(fd);
}

/* formatted_string_test
 *
 * Tests terminal write's ability to print special characters (\t, \b, \n)
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int formatted_string_test() {
  TEST_HEADER;
  int32_t fd = 0;
  int ret;
  int8_t str[] = "Hello\n\n\tWorld!!\b\n";
  int len = strlen(str);
  ret = terminal_write(fd, str, len);
  printf("If you see:\nHello\n\n\tWorld!\nThen you passed this test\n");
  if (ret == len) {
    return PASS;
  }
  return FAIL;

}

/* fish_string_test
 *
 * Fishy fishy time
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int fish_string_test() {
  TEST_HEADER;
  int32_t fd = 0;
  int ret;
  char* str = "/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n"
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
  if (ret == len) {
    return PASS;
  }
  return FAIL;

}

// Helper function for checking file names
int checkName(unsigned char* dentry_name, unsigned char* expected_name) {
  int size = strlen((char*) dentry_name);
  if (size > 32) size = 32; // If no null char
  return strncmp((char*) dentry_name, (char*) expected_name, size) == 0;
}

/*
 *   Read Current Directory dentry Test
 *
 *   DESCRIPTION: Tests getting the dentry of the current (".") directory
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_curr_dir_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) ".";
  printf("TESTING dentry by name . ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  if (result == -1) return FAIL;
  return checkName(dentry.name, fname) ? PASS : FAIL;
}

/*
 *   Read Too Long Filename dentry Test
 *
 *   DESCRIPTION: Tests reading dentry by 33 char filename where the first 32 match
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_too_long_file_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "verylargetextwithverylongname.txt";
  printf("TESTING dentry by name verylargetextwithverylongname.txt ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  return result == -1;
}

/*
 *   Read Max Length Filename dentry Test
 *
 *   DESCRIPTION: Tests reading dentry by 32 char filename
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_long_file_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "verylargetextwithverylongname.tx";
  printf("TESTING dentry by name verylargetextwithverylongname.tx ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  if (result == -1) return FAIL;
  return checkName(dentry.name, fname) ? PASS : FAIL;
}

/*
 *   Read Similar File (part 1) dentry Test
 *
 *   DESCRIPTION: Tests reading dentry of frame0.txt
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_similar_file_1_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "frame0.txt";
  printf("TESTING dentry by name frame0.txt ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  if (result == -1) return FAIL;
  return checkName(dentry.name, fname) ? PASS : FAIL;
}

/*
 *   Read Similar File (part 2) dentry Test
 *
 *   DESCRIPTION: Tests reading dentry of frame1.txt
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_similar_file_2_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "frame1.txt";
  printf("TESTING dentry by name frame1.txt ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  if (result == -1) return FAIL;
  return checkName(dentry.name, fname) ? PASS : FAIL;
}

/*
 *   Read Nonexistent dentry test
 *
 *   DESCRIPTION: Tests reading dentry of nonexistent filename
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_nonexistent_dentry_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "this_don't_exist.txt";
  printf("TESTING dentry by name this_don't_exist.txt ...");
  int32_t result = read_dentry_by_name(fname, &dentry);
  return result == -1 ? PASS : FAIL;
}

/*
 *   Read Dentry by index test
 *
 *   DESCRIPTION: Tests reading current directory by index 0
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_curr_dir_by_index_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t index = 0;
  printf("TESTING dentry by index . ...");
  int32_t result = read_dentry_by_index(index, &dentry);
  if (result == -1) return FAIL;
  return checkName(dentry.name, (unsigned char*) ".");
}

/*
 *   Read dentry by index out of bounds test
 *
 *   DESCRIPTION: Tests reading dentry by out of bounds index
 *   INPUTS: None
 *   OUTPUTS: None
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_out_of_bounds_dir_by_index_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t index = 100;
  printf("TESTING dentry at out-of-bounds index ...");
  int32_t result = read_dentry_by_index(index, &dentry);
  return result == -1;
}

/*
 *   Read file test
 *
 *   DESCRIPTION: Tests reading frame0.txt and compares it to what it should be
 *   INPUTS: None
 *   OUTPUTS: a fish
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_file_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "frame0.txt";
  read_dentry_by_name(fname, &dentry);

  char* solution = "/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\\n"
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

  uint32_t length = strlen(solution);
  uint8_t buf[length + 1];
  buf[length] = '\0';
  printf("TESTING reading frame0.txt\n");
  read_data(dentry.inode, 0, buf, length);
  printf("%s\n", buf);
  printf("...");

  return strncmp((char*) buf, solution, length) == 0 ? PASS : FAIL;
}

/*
 *   Read Big File test
 *
 *   DESCRIPTION: Tests reading very large file and checks the first and last 100 bytes
 *   INPUTS: None
 *   OUTPUTS: the first and last 5 bytes along with their actual values
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_big_file_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "verylargetextwithverylongname.tx";
  read_dentry_by_name(fname, &dentry);

  char* first100Solution = "very large text file with a very long name\n"
                           "123456789012345678901234567890123456789012345678901234567";
  char* last100Solution = "]\\{}|;':\",./<>?~!@#$%^&*()_+`1234567890-=[]\\{}|;':\",./<>?~!@#$%^&*()_+`1234567890-=[]\\{}|;':\",./<>?\n";

  uint32_t length = _inodes[dentry.inode].length;
  uint8_t buf[length + 1];
  buf[length] = '\0';
  printf("TESTING reading verylargetextwithverylongname.tx\n");
  read_data(dentry.inode, 0, buf, length);

  uint8_t* last100 = &buf[length - 100];

  printf("TESTING first 100 chars match...\n");
  printf("Solution first 5 bytes: 0x%x 0x%x 0x%x 0x%x 0x%x\n", first100Solution[0], first100Solution[1],
         first100Solution[2], first100Solution[3], first100Solution[4]);
  printf("    Read first 5 bytes: 0x%x 0x%x 0x%x 0x%x 0x%x\n...", buf[0], buf[1], buf[2], buf[3], buf[4]);
  if (strncmp((char*) buf, first100Solution, 100) != 0) return FAIL;
  PRINT_PASS;

  printf("TESTING last 100 chars match...\n");
  printf("Solution last 5 bytes: 0x%x 0x%x 0x%x 0x%x 0x%x\n", last100Solution[95], last100Solution[96],
         last100Solution[97], last100Solution[98], last100Solution[99]);
  printf("    Read last 5 bytes: 0x%x 0x%x 0x%x 0x%x 0x%x\n", last100[95], last100[96], last100[97], last100[98],
         last100[99]);
  return strncmp((char*) last100, last100Solution, 100) == 0 ? PASS : FAIL;
}

/*
 *   Read Executable Test
 *
 *   DESCRIPTION: Tests reading executable and compares the first and last chars to what they should be
 *   INPUTS: None
 *   OUTPUTS: the first and last chars along with what they should be
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
int read_exec_test() {
  TEST_HEADER;
  dentry_t dentry;
  uint8_t* fname = (unsigned char*) "pingpong";
  read_dentry_by_name(fname, &dentry);

  uint32_t length = _inodes[dentry.inode].length;
  uint8_t buf[length + 1];
  buf[length] = '\0';
  printf("TESTING reading pingpong\n");
  read_data(dentry.inode, 0, buf, length);

  int8_t* firstCharsSolution = "ELF";
  int8_t* lastCharsSolution = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  uint8_t firstBuf[20];
  memcpy(firstBuf, buf, 20);

  printf("First 20 chars: ");
  uint32_t i;
  for (i = 0; i < 20; i++) {
    printf("%c", firstBuf[i]);
  }
  printf("\n");
  printf("  Should match:  %s \n...", firstCharsSolution);
  if (strncmp((char*) &buf[1], firstCharsSolution, 3) != 0) return FAIL;
  PRINT_PASS;

  printf("Last 40 chars: ");
  for (i = 0; i < 40; i++) {
    printf("%c", buf[length - 40 + i]);
  }
  printf("\n");
  printf(" Should match:    %s \n...", lastCharsSolution);
  if (strncmp((char*) &buf[length - 37], lastCharsSolution, 36) != 0) return FAIL;
  return PASS;
}

/*
 *   Read File
 *
 *   DESCRIPTION: Tests reading frame0.txt
 *   INPUTS: filename to read
 *   OUTPUTS: the data in the file
 *   RETURN VALUE: PASS/FAIL
 *   SIDE EFFECTS: None
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
static char buf[100000];

int32_t read_file_by_name(uint8_t* s) {
  int j;
  int32_t read_bytes;
  for (j = 0; j < 100000; j++) {
    buf[j] = '\0';
  } // clear the buffer
  int length;
  dentry_t dentry;
  //corner case
  if (-1 == read_dentry_by_name(s, &dentry)) {
    printf("invalid file name! ");
    return -1;
  }

  length = _inodes[dentry.inode].length;
  printf("the file length: %d\n", length);
  //read data into the buffer
  read_bytes = read_data(dentry.inode, 0, (uint8_t*) buf, length);
  printf("the file succussfully read: %d\n", read_bytes);
  buf[length] = 0;

  int i;
  for (i = 0; i < 100000; i++) {
    // don't print null bytes
    if (buf[i] != 0) {
      //putc( buf[i]);
      printf("%c", buf[i]);
    }
    //putc(buf[i]);
  }
  char buffer[32 + 1];
  buffer[32] = 0;
  //just print the pre 32 bytes of the file name
  strncpy(buffer, (int8_t*) dentry.name, 32);
  printf("\nfile_name: %s\n", buffer);
  return 0;
}

/*
 *   read_file_by_name
 *   DESCRIPTION: read the data and print out the bytes successfully read
 *   INPUTS: the file name we want to read
 *           the beyond length to read
 *   OUTPUTS: the data in this file
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void check_read_file_by_name(uint8_t* s, int32_t outbound) {
  int j;
  int32_t read_bytes;
  for (j = 0; j < 100000; j++) {
    ((int8_t*) buf)[j] = '\0';
  } // clear the buffer
  int length;
  dentry_t dentry;
  //corner case
  if (-1 == read_dentry_by_name(s, &dentry)) {
    printf("invalid file name! ");
    return;
  }

  //length = filesys_get_data_size(dentry.inode);
  length = _inodes[dentry.inode].length;
  printf("the file length: %d\n", length);
  //read data into the buffer
  read_bytes = read_data(dentry.inode, 0, (uint8_t*) buf, length + outbound);
  printf("the file succussfully read: %d\n", read_bytes);
}

/*
*   build_fdarray
  *   DESCRIPTION: build_fdarray
  *   INPUTS: the file name
  *           fd index
  *   OUTPUTS: fdarray
  *   RETURN VALUE: file length
  *   SIDE EFFECTS: none
*/

int32_t build_fdarray(uint8_t* s, int32_t fd) {
  dentry_t dentry;
  //corner case
  if (-1 == read_dentry_by_name(s, &dentry)) {
    printf("invalid file name! ");
    return -1;
  }
  filearray[fd].inode_index = dentry.inode;
  filearray[fd].file_position = 0;
  return _inodes[dentry.inode].length;
}

/*
*   read_file_by_fd
*   DESCRIPTION: read the file
*   INPUTS: the file name we want to read
*           fd index we want to read
*   OUTPUTS: the data in this file
*   RETURN VALUE: none
*   SIDE EFFECTS: none
*/
static char buff[100000];

int32_t read_file_by_fd(int32_t fd, int32_t length) {

  int j;
  int32_t read_bytes;
  for (j = 0; j < 100000; j++) {
    ((int8_t*) buff)[j] = '\0';
  } // clear the buffer

  printf("the file length: %d\n", length);

  //read data into the buffer
  read_bytes = f_read(fd, (uint8_t*) buff, length);

  printf("the file succussfully read: %d\n", read_bytes);

  buff[read_bytes] = 0;

  int i;
  for (i = 0; i < 100000; i++) {
    // don't print null bytes
    if (buff[i] != 0) {
      printf("%c", buff[i]);
    }

  }

  return 0;
}

/*
*   read_file_by_fd
*   DESCRIPTION: read all in directory
*   INPUTS: none
*   OUTPUTS: all file names
*   RETURN VALUE: none
*   SIDE EFFECTS: none
*/
int32_t read_dir_all() {
  int j;
  int32_t read_bytes;
  //we don't have pcb now, so just set one fd's file_position to 0
  filearray[2].file_position = 0;

  printf("\nTotal File Count: %d\n", _boot_block.dentry_count);
  while (read_bytes != -1) {
    for (j = 0; j < 100; j++) {
      buff[j] = '\0';
    } // clear the buffer
    //read data into the buffer
    //for we does make the pcb right now, so just fd into any from 2-8
    read_bytes = d_read(2, (uint8_t*) buff, 32);

    if (read_bytes == 0) {
      continue;
    }
    buff[read_bytes] = 0;

    int i;
    for (i = 0; i < 100; i++) {
      // don't print null bytes
      if (buff[i] != 0) {
        printf("%c", buff[i]);
      }
    }
    printf("\n");
  }


  return 0;
}

/*
 *   Pretty Print all files, types, and sizes
 *
 *   INPUTS: None
 *   OUTPUTS: All Files, types, and sizes
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 *   Coverage: Filesystem
 *   Files: filesystem.c/h
 */
void pretty_print_all() {
  uint32_t i, j;
  printf("Total file count: %d\n\n", _boot_block.dentry_count);
  set_attrib(0x06);
  printf("File Name                               File Type        File Size     \n");
  printf("---------                               ---------        ---------     \n");
  for (i = 0; i < _boot_block.dentry_count; i++) {
    dentry_t dentry = _boot_block.dentries[i];
    inode_t inode = _inodes[dentry.inode];
    set_attrib(dentry.file_type == 2 ? 0x03 : 0x05);
    for (j = 0; j < 32; j++) {
      int8_t c = dentry.name[j];
      printf("%c", c == '\0' ? ' ' : c);
    }
    set_attrib(0x03);
    printf("        %d                %d\n", dentry.file_type, inode.length);
  }
  set_attrib(0x07);
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

// set these macros to 1 to run tests
// you would still need to set individual tests within groups
// tabs represent hierarchy
#define cp1_tests 0
    #define cp1_idt_test 0
    #define cp1_rtc_test 0
    #define cp1_paging_test 0
    #define cp1_exception_test 0 

#define cp2_tests 1
    #define cp2_rtc_test 1
    #define cp2_terminal_test 0
    #define cp2_filesys_test 0
/* Test suite entry point */
void launch_tests() {
    clear();
    reset_text_cursor();

    /* TESTS FOR CP 1 */
    #if cp1_tests
    // idt test
    #if cp1_idt_test
    TEST_OUTPUT("idt_test", idt_test());
    #endif

    // rtc changing frequency test
    #if cp1_rtc_test
    rtc_freq_test();
    #endif

    // paging tests
    #if cp1_paging_tests
    TEST_OUTPUT("paging_struct_test", paging_struct_test());
    TEST_OUTPUT("paging_vga_test", paging_vga_test());
    TEST_OUTPUT("paging_kernel_test", paging_kernel_test());
    TEST_OUTPUT("paging upper and lower bound test", paging_check_bounds());
    page_overflow_test();
    page_fault_exception_test();
    #endif

    // zero-division exception
    #if cp1_exception_test
    divide_zero_test();
    #endif
    #endif

///////////////////////////////////////////////////////////////////////////////
    /* TESTS FOR CP 2 */
    #if cp2_tests
    #if cp2_rtc_test
    TEST_OUTPUT("rtc_freq_bounds_test", rtc_freq_bounds_test());
    add_delay();
    clear();
    reset_text_cursor();
    TEST_OUTPUT("rtc_functions_test", rtc_functions_test());
    #endif

    #if cp2_terminal_test
    TEST_OUTPUT("Terminal open and close returns -1", terminal_open_close());
    TEST_OUTPUT("Terminal overflow: read", terminal_read_of());
    TEST_OUTPUT("Terminal test: write", terminal_write_test());
    TEST_OUTPUT("Formatted string", formatted_string_test());
    add_delay();
    add_delay();
    TEST_OUTPUT("Formatted string: fish", fish_string_test());
    terminal_readwrite_test_Delay();
    terminal_readwrite_test_inf();
    #endif

    #if cp2_filesys_test
    // TEST_OUTPUT("read_curr_dir_dentry_test", read_curr_dir_dentry_test());
    //TEST_OUTPUT("read_very_long_file_test", read_too_long_file_dentry_test());
    // TEST_OUTPUT("read_very_long_file_test", read_long_file_dentry_test());
    // TEST_OUTPUT("read_similar_file_1_dentry_test", read_similar_file_1_dentry_test());
    // TEST_OUTPUT("read_similar_file_2_dentry_test", read_similar_file_2_dentry_test());
    // TEST_OUTPUT("read_nonexistent_dentry_test", read_nonexistent_dentry_test());
    // TEST_OUTPUT("read_curr_dir_by_index_test", read_curr_dir_by_index_test());
    // TEST_OUTPUT("read_out_of_bounds_dir_by_index_test", read_out_of_bounds_dir_by_index_test());
    // TEST_OUTPUT("read_file_test", read_file_test());
    // TEST_OUTPUT("read_big_file_test", read_big_file_test());
    // TEST_OUTPUT("read_exec_test", read_exec_test());
    // read_file_by_name((uint8_t*)"pingpong");
    // check_read_file_by_name((uint8_t*)"pingpong",100);
    // read_file_by_fd(2,  build_fdarray((uint8_t*)"pingpong",2));
    // read_dir_all();
    pretty_print_all();
    #endif
    #endif
///////////////////////////////////////////////////////////////////////////////
}

