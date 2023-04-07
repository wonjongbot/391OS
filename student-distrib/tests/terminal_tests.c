//
// Created by hijoe on 4/6/2023.
//

#include "terminal_tests.h"

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
  terminal_open((uint8_t*)3);
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
  uint8_t* file = 0x0;
  if (terminal_close(fd) == -1 && terminal_open(file) == -1) {
    return PASS;
  }
  return FAIL;
}

/* Terminal_null test
 *
 * Tests the behavior of nullcheck of terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int terminal_null() {
  TEST_HEADER;
  int32_t fd = 0;
  if (terminal_read(fd, (void*)0, 0) == -1 && terminal_write(fd, (void*)0, 0) == -1) {
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
  uint8_t* file = 0x0;
  int32_t fd = 0;
  set_attrib(0x0b);
  printf("[*] Sandbox testing: Type `exit` to leave this test\n");
  set_attrib(0x07);
  char exit_flag[] = "exit\n";
  terminal_open(file);
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
