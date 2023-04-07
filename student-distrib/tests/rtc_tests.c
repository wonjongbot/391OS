//
// Created by hijoe on 4/6/2023.
//

#include "rtc_tests.h"

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

/* rtc_null test
 *
 * Tests the behavior nullcheck of rtc
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: terminal
 * Files: terminal.h/c
 */
int rtc_null() {
  TEST_HEADER;
  int32_t fd = 0;
  if (rtc_read(fd, (void*)0, 0) == -1 && rtc_write(fd, (void*)0, 0) == -1) {
    return PASS;
  }
  return FAIL;
}
