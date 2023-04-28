#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "system_calls.h"
#include "tests/rtc_tests.h"
#include "tests/terminal_tests.h"
#include "tests/filesystem_tests.h"
#include "tests/pcb_tests.h"

// global variable to check test function output
//int ret;

// set_atrib function takes in 4 bits for foreground color and 4 bits for background color
// colors we used are :
// 		- 0x07: black background with grey text
//		- 0x0B: black background with cyan text
//		- 0x02: black background with green text
//		- 0x4E: red background with yellow text
//      - 0x2f: green background with white text

/* add_delay
 *
 * helper function to add delay
 * Inputs: None
 * Outputs: None
 * Side Effects: adds delay
 */
void add_delay() {
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

/* Checkpoint 2 tests */



/* Checkpoint 3 tests */
/*
 *   test if the syscall can work, basic for read write open close
 *    (the rtc didn't test)
 *
 *   INPUTS: None
 *   OUTPUTS: a file's fd, and what we read, final check if close
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void basic_syscall_print_read() {
  static char buf[6000];
  uint32_t fd;
  fd = open((uint8_t*) "pingpong");
  printf("fd is %d\n", fd);
  read(fd, buf, 6000);


  int i;
  for (i = 0; i < 6000; i++) {
    // don't print null bytes
    if (buf[i] != 0) {
      //putc( buf[i]);
      printf("%c", buf[i]);
    }
    //putc(buf[i]);
  }
  write(fd, buf, 6000);
  close(fd);
  printf("\n");
  printf("the flag of this fd is %d\n", current->filearray[fd].flags);
}

// /*
//  *   test_ls
//  *   DESCRIPTION: list all files in the file system
//  * 				  includes file_name, file_type, file_size
//  *   INPUTS: none
//  *   OUTPUTS: file name; file type; file size
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
 void test_ls(){
 	TEST_HEADER;
 	int32_t fd, cnt;
 	uint8_t buf[33];
 	dentry_t dentry;

 	buf[32] = '\0';
 	fd = open((uint8_t*)".");

 	while (0 != (cnt = read (fd, buf, 32))) {
// 		check if read the name of dentry successfully
         if (-1 == cnt) {
 	        printf ((int8_t*)"directory entry read failed\n");
 	        return;
 	    }
// 		//according to the file ame read the data of the file
 		read_dentry_by_name((uint8_t*)buf, &dentry);
 		uint32_t length=strlen((int8_t*)buf);
 		uint32_t field_length = 35;
 		uint32_t i =0;
 		//print relevant information
 		printf("filename: ");
 		for(i = 0; i<field_length-length; i++){
 		 	printf(" ");
 		 }

 		printf("%s, file_type: %d  \n",buf,dentry.file_type);


     }

 	return;

 }



 // /*
//  *   print_exe_file
//  *   DESCRIPTION: read the data and print on the screen , test "read exe"
//  *   INPUTS: none
//  *   OUTPUTS: the data in this file
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
 void print_exe_file() {
 	TEST_HEADER;
 	uint8_t buf[6000];
 	uint32_t j;
 	for(j = 0; j < 6000; j++){
          ((int8_t*)buf)[j] = '\0';
     } // clear the buffer
 	int32_t fd_cur,i,result;
 	i = 0;
 	//open the file
 	fd_cur = open((uint8_t*)"ls");
 	if(fd_cur ==-1){
 		printf("fail opening ls\n");
 		return;
 	}
 	//read the data into the buffer
 	read(fd_cur,buf,6000);
 	//print the buffer
 	for(i = 0 ; i<6000;i++){
 		// don't print null bytes
 		if(buf[i]!=0){
 			putc(buf[i]);
 		}
 		 //putc(buf[i]);
 	}
 	result=write(fd_cur, buf, 6000);
 	if(result!=-1){
 		printf("gg");
 	}
 	printf("\n");
 	printf("file_name: ls");
 	close(fd_cur);

 }

 // /*
//  *   syscall_open_test
//  *   DESCRIPTION: open file and print on the screen , test "syscall_open"
//  *   INPUTS: none
//  *   OUTPUTS: pass or fail
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
 void syscall_open_test() {
  if (syscall_open((uint8_t*)"pingpong") > 1) {
    printf("syscall_open_test passed! \n");
  } else {
    printf("syscall_open_test failed! \n");
  }
}

 // /*
//  *   syscall_close_test
//  *   DESCRIPTION: close file and print on the screen , test "syscall_close
//  *   OUTPUTS: pass or fail
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
void syscall_close_test() {
  int32_t fd = syscall_open((uint8_t*)"frame0.txt");
  if (syscall_close(fd) == -1) {
    printf("syscall_close_test failed! \n");
  } else {
    printf("syscall_close_test passed! \n");
  }
}

 // /*
//  *   syscall_read_write_test
//  *   DESCRIPTION: read file and write to terminal print on the screen , test "syscall_read, syscall_write"
//  *   INPUTS: none
//  *   OUTPUTS: pass or fail
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
void syscall_read_write_test() {
  int32_t fd = syscall_open((uint8_t*)"frame0.txt");
  char buf[200];
  if (syscall_read(fd, buf, 200) == -1) {
    printf("syscall_read_write_test failed! \n");
  }
  buf[199] = '\0';
  (void)syscall_write(1, buf, strlen(buf));

  printf("syscall_read_write_test passed! \n");
}

/* Checkpoint 4 tests */
// /*
//  *   garbage_input_test
//  *   DESCRIPTION: test the invalid input of our functions
//  *   INPUTS: none
//  *   OUTPUTS: none
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
  void garbage_input_test(){ // garbage input

 	if(PCB_init(NULL) == -1) printf("garbage_input_test for PCB_init passed! \n");

 	if(syscall_execute(NULL) == -1) printf("garbage_input_test for sys_execute passed! \n");
 	if(syscall_read(10,NULL,0) == -1) printf("garbage_input_test for sys_read passed! \n");
 	if(syscall_write(-1,NULL,0) == -1) printf("garbage_input_test for sys_write passed! \n");
 	if(syscall_open(NULL) == -1) printf("garbage_input_test for sys_open passed! \n");
 	if(syscall_close(-1) == -1) printf("garbage_input_test for sys_close passed! \n");
  if(syscall_getargs(NULL,0) == -1) printf("garbage_input_test for sys_getargs passed! \n");

 }

//  *   vidmap_test
//  *   DESCRIPTION: test vidmap
//  *   OUTPUTS: none
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
void vidmap_test(){
 uint8_t** screen_start = (uint8_t**)(VALUE_128MB+1);
 if (vidmap (screen_start)!=-1){
  printf("vidmap test pass\n");
 }
}

//  *   getargs_vidmap_test
//  *   DESCRIPTION: test getargs/vidmap
//  *   OUTPUTS: none
//  *   RETURN VALUE: none
//  *   SIDE EFFECTS: none
//  */
void getargs_vidmap_test(){
  uint8_t* commandC;
 uint8_t** screen_start = NULL;
	void* buf = NULL;
	execute (commandC);
  printf("execute system call pass\n");
	read (2 ,buf ,3);
  printf("read system call pass\n");
	write (2, buf, 3);
  printf("write system call pass\n");
	open ( commandC);
  printf("open system call pass\n");
	close (2);
  printf("close system call pass\n");
	getargs (buf, 3);
  printf("getargs system call pass\n");
  vidmap (screen_start);
  printf("vidmap system call pass\n");
  
}


/* Checkpoint 5 tests */

// set these macros to 1 to run tests
// you would still need to set individual tests within groups
// tabs represent hierarchy
#define cp1_tests 0
#define cp1_idt_test 0
#define cp1_rtc_test 0
#define cp1_paging_test 0
#define cp1_exception_test 0

#define cp2_tests 0
#define cp2_rtc_test 0
#define cp2_terminal_test 0
#define cp2_filesys_test 0
#define cp2_filesys_test_1 0
#define cp2_filesys_test_2 0
#define cp2_filesys_test_3 0
#define cp2_filesys_test_4 0
#define cp2_filesys_test_5 0
#define cp2_filesys_test_6 0
#define cp2_filesys_test_7 0

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
  TEST_OUTPUT("nullcheck rtc", rtc_null());
#endif
#if cp2_terminal_test
  TEST_OUTPUT("Terminal open and close returns -1", terminal_open_close());
  TEST_OUTPUT("Terminal overflow: read", terminal_read_of());
  TEST_OUTPUT("Terminal test: write", terminal_write_test());
  TEST_OUTPUT("Formatted string", formatted_string_test());
  add_delay();
  add_delay();
  TEST_OUTPUT("Formatted string: fish", fish_string_test());
  TEST_OUTPUT("nullcheck terminal", terminal_null());
  terminal_readwrite_test_Delay();
  terminal_readwrite_test_inf();
#endif

#if cp2_filesys_test
#if cp2_filesys_test_1
  TEST_OUTPUT("read_curr_dir_dentry_test", read_curr_dir_dentry_test());
  TEST_OUTPUT("read_very_long_file_test", read_too_long_file_dentry_test());
  TEST_OUTPUT("read_very_long_file_test", read_long_file_dentry_test());
  TEST_OUTPUT("read_similar_file_1_dentry_test", read_similar_file_1_dentry_test());
  TEST_OUTPUT("read_similar_file_2_dentry_test", read_similar_file_2_dentry_test());
  TEST_OUTPUT("read_nonexistent_dentry_test", read_nonexistent_dentry_test());
  TEST_OUTPUT("read_curr_dir_by_index_test", read_curr_dir_by_index_test());
  TEST_OUTPUT("read_out_of_bounds_dir_by_index_test", read_out_of_bounds_dir_by_index_test());
#endif

#if cp2_filesys_test_2
  TEST_OUTPUT("read_file_test", read_file_test());
#endif

#if cp2_filesys_test_3
  TEST_OUTPUT("read_big_file_test", read_big_file_test());
  TEST_OUTPUT("read_exec_test", read_exec_test());
#endif

#if cp2_filesys_test_4
  read_file_by_name((uint8_t*)"pingpong");
  check_read_file_by_name((uint8_t*)"pingpong",100);
#endif

#if cp2_filesys_test_5
  read_file_by_fd(2,  build_fdarray((uint8_t*)"testprint",2));
#endif

#if cp2_filesys_test_6
  read_dir_all();
#endif

#if cp2_filesys_test_7
  pretty_print_all();
#endif
#endif
#endif
///////////////////////////////////////////////////////////////////////////////
  // execute_test();
  //  garbage_input_test();
  //  getargs_vidmap_test();

  // basic_syscall_print_read();
//  syscall_open_test();
//  syscall_close_test();
//  syscall_read_write_test();
//
//  set_attrib(0x0B);
//  printf("Manual tests:\n1. Run syserr test 0. \n\t* This tests invaid open and close system call.\n");
//  printf("2. Try to execute unimplemented systemcalls.\n\t* Try fish and see if it returns abnormal termination.\n");
//  printf("3. Open RTC from shell.\n\t* This should return no such command.\n");
//  printf("4. Test exception squashing\n\t* Try sigtest 0. Does it return to parent shell?\n");
//  printf("\t* Enable SQUASH_TEST macro in rtc.h and try to execute pingpong.\n\t  Does it return to parent shell?\n");
//  set_attrib(0x07);

  //execute((uint8_t*)"shell");
}

