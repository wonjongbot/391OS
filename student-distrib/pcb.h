#ifndef PROCESS_H
#define PROCESS_H

#include "x86_desc.h"
#include "system_calls.h"
#include "filesystem.h"
#include "lib.h"

#define MAX_THREAD_FOR_PCB 2
#define ARGV_MAX_LEN 100
#define MAX_PROCESS_NUM 6

#define current current_thread_PCB()

typedef struct PCB{
    uint32_t pid;   // 0 to 5 
    filed filearray[FILEARR_SIZE]; // fd 
    uint32_t status;
    uint32_t PCB_pos;
    uint32_t argc;   
    uint8_t argv[ARGV_MAX_LEN + 1];
    struct PCB* parent; 

    //need to add terminal information (not familiar of this part);

    // TSS
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    // Memory Map
    uint32_t physical_mem_start;
    // Shell
    uint32_t shell_flag;

    // need Signal for cp6
    
    // Thread
    int32_t thread_flag;
    int32_t thread_num;
    int32_t global_thread_index;
    int32_t threads[MAX_THREAD_FOR_PCB];
}PCB;

int32_t pid_alloc();

//point to the current process
inline PCB* current_thread_PCB();





#endif
