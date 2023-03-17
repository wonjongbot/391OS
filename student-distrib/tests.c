#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
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
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

//add more tests here

 void page_fault_exception_test(){
 	TEST_HEADER;
 	int* p = NULL;
 	int a;
 	a = *p;
 }

 void page_overflow_test(){
 	TEST_HEADER;
 	int* p = (int*)(0x800000 + 0x100000);
 	int a;
 	a= *p;
 }

 int paging_vga_test(){
 	TEST_HEADER;
 	int *a = (int *)0x000B8000;
 	int b;
 	b = *a;
 	return 1;
 }

 int paging_kernal_test(){
 	TEST_HEADER;
 	int *a = (int *)((1<<22)+12);
 	int b;
 	b = *a;
 	return 1;
 }

  
 int paging_struct_test(){
 	TEST_HEADER;
 	int i;
 	int flag = 1;
 	for(i=0;i<PAGE_DIC_ENTRY;i++){
 		flag = flag && page_directory[i].val;
 	}
 	for(i=0;i<PAGE_TAB_ENTRY;i++){
 		flag = flag && page_table0[i].val;
 	}
 	return flag!=0;
 }

int divide_zero_test(){
	int a = 0;
	int b = 0;
	int c = a / b;
	return c;
}

void rtc_freq_test(){
	unsigned i, j;
	clear();
	for(i = 1; i < 1025; i = i * 2){
		printf("FREQUENCY IS %dHz\n", i);
		rtc_set_freq(i);
		for(j = 0; j < 600000000; j++);
		for(j = 0; j < 400000000; j++);
		printf("\n");
	}
	printf("RTC TEST DONE\n");
	rtc_set_rate(0);
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// launch your tests here
	// idt test
	TEST_OUTPUT("idt_test", idt_test());

	// rtc changing frequency test
	rtc_freq_test();

	// paging tests
	TEST_OUTPUT("paging_struct_test", paging_struct_test());
	TEST_OUTPUT("paging_vga_test", paging_vga_test());
	TEST_OUTPUT("paging_kernal_test", paging_kernal_test());
	page_overflow_test();
	page_fault_exception_test();

	// zero-division exception
	//divide_zero_test();
}

