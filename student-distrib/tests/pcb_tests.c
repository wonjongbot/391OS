//
// Created by hijoe on 4/6/2023.
//

#include "pcb_tests.h"

void execute_test(){
  int ret;
  printf("Opening executable pingpong\n");
  ret = execute((uint8_t*)"pingpong");
  if(ret >= 0 && ret < 256){
    printf("Success: Executable opened\n");
  }
  else if(ret == 256){
    printf("Fail: Exception triggered\n");
  }
  else if(ret == -1){
    printf("Fail: Returned -1\n");
  }

  printf("Opening nullptr\n");
  ret = execute(NULL);
  if(ret >= 0 && ret < 256){
    printf("Success: Executable opened\n");
  }
  else if(ret == 256){
    printf("Fail: Exception triggered\n");
  }
  else if(ret == -1){
    printf("Fail: Returned -1\n");
  }
}


