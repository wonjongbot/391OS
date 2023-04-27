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
#define get_pcb_from_pos(pcb_pos) ((pcb_t*)((PAGE_4MB_VAL << 1) - (pcb_pos + 1) * (PAGE_4KB_VAL << 1)))
#define get_kernel_stack_from_pos(pcb_pos) ((uint32_t)(((uint32_t)get_pcb_from_pos(pcb_pos)) + (PAGE_4KB_VAL << 1) - 4))
typedef struct pcb {
  uint32_t pid;   // 0 to 5
  filed filearray[FILEARR_SIZE]; // fd
  uint32_t status;
  uint32_t argc;
  uint8_t argv[ARGV_MAX_LEN + 1];
  struct pcb* parent;


#if (muti_terminal==1)
  terminal* terminal;
#endif

  // TSS
  uint32_t esp;
  uint32_t ebp;
  uint32_t eip;

  // saved values for halt
  uint32_t save_ebp;
  uint32_t save_esp;

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
} pcb_t;

int32_t pid_alloc();

int32_t pid_peek();

void pid_dealloc(int32_t pid);

void unload(pcb_t* pcb);

//point to the current process
inline pcb_t* current_thread_PCB();

int32_t PCB_init(pcb_t* pcb);


#endif
