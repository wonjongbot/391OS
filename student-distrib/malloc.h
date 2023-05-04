#ifndef _MALLOC_H
#define _MALLOC_H

#include "system_calls.h"
#include "lib.h"
#include "paging.h"
#include "filesystem.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"

#include "sound.h"
#include "pit.h"

#define BLOCK_NUM 1024 
#define SPCAE_START 56 //  starts from 56*4=224M
#define SPCAE_END 80   //  ends from 64*4=256M
#define FOURKB 4095  // bit calculation is faster
#define KALLOC_ALLOWED BLOCK_NUM * (SPCAE_END - SPCAE_START) // Totally we can allocate at most KALLOC_ALLOWED times of allocation.

int8_t is_pre[SPCAE_END - SPCAE_START][BLOCK_NUM]; // 0 is unavailable, 1 is available.
int32_t next_ptr[(SPCAE_END - SPCAE_START) * BLOCK_NUM]; // points to the next page
int32_t record[KALLOC_ALLOWED]; // recording the selecting link, substitute the linked list.

int32_t base_list[KALLOC_ALLOWED];
int32_t base_cnt;

/* helper function, setting up paging when it is allocated */
extern void set_paging(int32_t index);

/* helper function, undo paging when it is freed */
extern void free_paging(int32_t index);

/* Initialize the array used for memory allocation in kernel. */
extern void kmalloc_init();

/* Memory allocation from the kernel */
extern void* kmalloc(int32_t nbytes);

/* Memory free from the kernel */
extern void kfree(void* base_addr);

#endif
