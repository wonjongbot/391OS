//
// Created by hijoe on 4/6/2023.
//

#ifndef MP3_FILESYSTEM_TESTS_H
#define MP3_FILESYSTEM_TESTS_H

#include "../filesystem.h"
#include "../lib.h"
#include "../tests.h"
#include "../types.h"

int checkName(unsigned char* dentry_name, unsigned char* expected_name);
int read_curr_dir_dentry_test();
int read_too_long_file_dentry_test();
int read_long_file_dentry_test();
int read_similar_file_1_dentry_test();
int read_similar_file_2_dentry_test();
int read_nonexistent_dentry_test();
int read_curr_dir_by_index_test();
int read_out_of_bounds_dir_by_index_test();
int read_file_test();
int read_big_file_test();
int read_exec_test();
int32_t read_file_by_name(uint8_t* s);
void check_read_file_by_name(uint8_t* s, int32_t outbound);
int32_t build_fdarray(uint8_t* s, int32_t fd);
int32_t read_file_by_fd(int32_t fd, int32_t length);
int32_t read_dir_all();
void pretty_print_all();

#endif //MP3_FILESYSTEM_TESTS_H
