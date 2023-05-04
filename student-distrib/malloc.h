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
#define SPCAE_END 80   //  ends from 80*4=320M
#define KALLOC_ALLOWED BLOCK_NUM * (SPCAE_END - SPCAE_START) 

int8_t mem_avl[SPCAE_END - SPCAE_START][BLOCK_NUM]; // 0 is unavailable, 1 is available.
int32_t next_ptr[(SPCAE_END - SPCAE_START) * BLOCK_NUM]; // points to the next page
int32_t record[KALLOC_ALLOWED]; 

int32_t base_list[KALLOC_ALLOWED];
int32_t base_num;


/* Initialize the array used for memory allocation in kernel. */
extern void kmalloc_init();

/* Memory allocation from the kernel */
extern void* kmalloc(int32_t nbytes);

/* Memory free from the kernel */
extern void kfree(void* base_addr);

#endif
