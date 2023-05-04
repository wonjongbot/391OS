
#include "malloc.h"
#define PAGE_BIT 12

/*
*   void kmalloc_init()
*       - Initialize the array used for memory allocation 
*   Input: None
*   Output: None
*/
void kmalloc_init(){
    int32_t i, j;
    for (i = 0; i < SPCAE_END- SPCAE_START; i++){
        for (j = 0; j < BLOCK_NUM; j++){
            mem_avl[i][j] = 1;
        }
    }
}

/*
*   void kmalloc(int32_t nbytes)
*       - Memory allocation from kernel
*   Input: nbytes - bytes to allocate in the kernel
*   Output: Return NULL for failure, return the allocated base address for success
*/
void* kmalloc(int32_t nbytes){
    int32_t page_num = (nbytes >> PAGE_BIT) + ((nbytes & ((1<<PAGE_BIT)-1)) > 0); 
    int32_t i, j;
    int32_t find = 0;
    int32_t cnt = 0;
    int32_t base; 

    for (i = SPCAE_START; i < SPCAE_END; i++){
        for (j = 0; j < BLOCK_NUM; j++){
            if (mem_avl[i - SPCAE_START][j] == 1){
                record[cnt++] = (i - SPCAE_START) * BLOCK_NUM + j;
                if (cnt == page_num){
                    record[cnt] = -1; 
                    find = 1; // found space to allocate
                    break;
                }
            }
        }
        if (find==1) break; 
    }
    if (find==0){
        printf("Memory allocated failed.\n");
        return NULL;
    }

    for (i = 0; i < cnt; i++){
        mem_avl[record[i] >> 10][record[i] & (BLOCK_NUM-1)] = 0;
        page_table2[record[i]].present = 1;
        next_ptr[record[i]] = record[i + 1];
    }

    base = record[0] + SPCAE_START * BLOCK_NUM;
    for (i = 0; i < KALLOC_ALLOWED; i++)
        if (base_list[i] == 0){
            base_list[i] = base;
            base_num++;
            break;
        }

    printf("New memory block allocated, started at %x. Block Size: %d KB\n", base << PAGE_BIT, page_num * 4); // 4KB for each page.
    return (void*) (base << PAGE_BIT);
}

/*
*   void kfree(void* base_addr)
*       - Memory free from kernel
*   Input: base_addr - base address for free
*   Output: None
*/
void kfree(void* base_addr){
    int32_t i,j;
    int32_t page_id = (int32_t)base_addr >> PAGE_BIT;
    int32_t free_flag = 0;
    for (i = 0; i < base_num; i++)
        if (base_list[i] == page_id){
            base_list[i] = 0;
            free_flag = 1;
            break;
        }
    if (free_flag==0){ 
        printf("Memory free failed.\n");
        return;
    }

    i = page_id - BLOCK_NUM * SPCAE_START;
    for(j=0;i!=-1;j++){ 
        mem_avl[(i >> 10) - SPCAE_START][i & (BLOCK_NUM-1)] = 1;      
        i = next_ptr[i];    //jump along the list
    }
    printf("Memory block stared at %x is freed. Freed size: %d KB\n", base_addr, j * 4); // 4KB for each page
}
