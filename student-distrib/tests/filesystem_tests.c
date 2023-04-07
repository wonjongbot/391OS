//
// Created by hijoe on 4/6/2023.
//

#include "filesystem_tests.h"

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

filed filearray[8];
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

