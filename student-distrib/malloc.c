
#include "malloc.h"
#define PAGE_BIT 12
/*
*   void set_paging(int32_t index)
*   Input: index - which page is present
*   Output/Side effect: None
*/
void set_paging(int32_t index){
    page_table2[index].present = 1;
}

/*
*   void free_paging(int32_t index)
*   Input: index - which page won't present
*   Output/Side effect: None
*/
void free_paging(int32_t index){
    page_table2[index].present = 0;
}

/*
*   void kmalloc_init()
*       - Initialize the array used for memory allocation in kernel.
*   Input: None
*   Output: None
*   Side effect: Mark those available address.
*/
void kmalloc_init(){
    /* Check all the present space. */
    int32_t i, j;
    for (i = 0; i < SPCAE_END- SPCAE_START; i++){
        for (j = 0; j < BLOCK_NUM; j++){
            is_pre[i][j] = 1;
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
    int32_t page_num = (nbytes >> PAGE_BIT) + ((nbytes & FOURKB) > 0); 
    int32_t i, j;
    int32_t find = 0;
    int32_t cnt = 0;
    int32_t base; 

    for (i = SPCAE_START; i < SPCAE_END; i++){
        for (j = 0; j < BLOCK_NUM; j++){
            if (is_pre[i - SPCAE_START][j] == 1){
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
        is_pre[record[i] >> 10][record[i] & (BLOCK_NUM-1)] = 0;
        set_paging(record[i]);
        next_ptr[record[i]] = record[i + 1];
    }

    base = record[0] + SPCAE_START * BLOCK_NUM;
    for (i = 0; i < KALLOC_ALLOWED; i++)
        if (base_list[i] == 0){
            base_list[i] = base;
            base_cnt++;
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
    int32_t i;
    int32_t page_id = (int32_t)base_addr >> PAGE_BIT;
    int32_t free_flag = 0;
    for (i = 0; i < base_cnt; i++)
        if (base_list[i] == page_id){
            base_list[i] = 0;
            free_flag = 1;
            break;
        }
    if (!free_flag){ 
        printf("Memory free failed.\n");
        return;
    }

    i = page_id - BLOCK_NUM * SPCAE_START;
    int32_t counter = 0;
    while (i != -1){ 
        is_pre[(i >> 10) - SPCAE_START][i & (BLOCK_NUM-1)] = 1;      
        i = next_ptr[i];    //jump along the list
        counter++;
    }
    printf("Memory block stared at %x is freed. Freed size: %d KB\n", base_addr, counter * 4); // 4KB for each page
}
