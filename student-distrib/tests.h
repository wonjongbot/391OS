#ifndef TESTS_H
#define TESTS_H

#define PASS 1
#define FAIL 0


/* format these macros as you see fit */
#define TEST_HEADER    \
    set_attrib(0x0B);\
    printf("[TEST] Running %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__);\
    set_attrib(0x07);
#define TEST_OUTPUT(name, result) \
    int ret;                              \
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

void add_delay();
// test launcher
void launch_tests();

#endif /* TESTS_H */
