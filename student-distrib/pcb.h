#ifndef PROCESS_H
#define PROCESS_H

#include "x86_desc.h"
#include "system_calls.h"
#include "filesystem.h"
#include "lib.h"
#include "terminal.h"

#define MAX_THREAD_FOR_PCB 2
#define ARGV_MAX_LEN (kb_buf_size - 3) // keyboard buf length - 2 for hypothetical 1 char filename + space + null/newline char
#define MAX_PROCESS_NUM 6

#define current current_thread_PCB()

typedef struct pcb {
  uint32_t pid;                     // Current PCB's running process ID.
  filed filearray[FILEARR_SIZE];    // fd array for each process
  uint32_t status;                  // Status variable to check if this PCB is used or not
            //  uint32_t argc;                    // unused
  uint8_t argv[ARGV_MAX_LEN + 1];   // string of argument portion of the command
  struct pcb* parent;               // pointer to the parent PCB

  //need to add terminal information (not familiar of this part);

  // Information saved for scheduling purposes
  uint32_t esp;
  uint32_t ebp;

  // For saving/restoring TSS.ESP0 and TSS.SS0 when switching cntext
  uint32_t esp0;
  uint32_t ss0;

  // saved values for halt
  uint32_t save_ebp;
  uint32_t save_esp;

  // Start of the user space memory
  uint32_t physical_mem_start;
  // Shell
  uint32_t shell_flag;

  // need Signal for cp6
} pcb_t;

int32_t pid_alloc();

int32_t pid_peek();

void pid_dealloc(int32_t pid);

void unload(pcb_t* pcb, int32_t ret);

//point to the current process
inline pcb_t* current_thread_PCB();

pcb_t* PCB(uint32_t pid);

int32_t PCB_init(pcb_t* pcb);

void print_proc();

#endif
