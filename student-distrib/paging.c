#include "paging.h"


/*
 *   init_paging
 *   DESCRIPTION: used to init and open paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: will change page_directory and page_table0 array, will change the control registers, and will enable paging
 */
void init_paging(){
    uint32_t i;
    uint32_t kernel_index,vga_table_index,vga_dic_index; // declare variables for table and directory index
    // modex_table_index;

    //initialize the page directory
    //page_directory[0] (4kb)
    for(i=0;i<PAGE_DIC_MAX;i++){
        page_directory[i].val = 0;  // clean all
        page_directory[i].rw = 1;   // set rw flag
    }

    //initialize the page table
    for(i=0;i<PAGE_TAB_MAX;i++){
        page_table0[i].val = 0;     // clean all
        page_table0[i].present=0;
        page_table0[i].rw = 1;      // set rw flag
        page_table0[i].base_addr = i;   // set the base address as the entry index aligned
        page_table1[i].val = 0;     // clean all
        page_table1[i].present=0;
        page_table1[i].rw = 1;      // set rw flag
        page_table1[i].base_addr = i;   // set the base address as the entry index aligned
    }

    /* we would uncomment this if we want the first page to be available*/
    // page_directory[0].present = 1;
    // the value below is zero so we probably don't even need this
    // page_directory[0].base_addr=((uint32_t)page_table0 & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;

    // Set the kernel page (4MB)
    kernel_index=dir_entry(KERNEL_ADDR);
    page_directory[kernel_index].present=1;
    page_directory[kernel_index].rw=1;
    page_directory[kernel_index].ps=1;
    page_directory[kernel_index].global=1;
    page_directory[kernel_index].base_addr=KERNEL_ADDR>>TABLE_ADDRESS_SHIFT;

    // Set the page for VGA text mode, first set the page directory entry
    // Then, set the page_table0 with appropriate values
    //get index for the page table and page directory for VGA text mode page (ADDR 0xb8000)
    vga_table_index=page_entry(VGA_TEXT_BUF_ADDR);
    vga_dic_index=dir_entry(VGA_TEXT_BUF_ADDR);

    page_directory[vga_dic_index].present=1;
    page_directory[vga_dic_index].rw=1;
    page_directory[vga_dic_index].global=1;
    page_directory[vga_dic_index].base_addr=((uint32_t)page_table0 & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;

    page_table0[vga_table_index].present=1;
    page_table0[vga_table_index].rw=1;
    page_table0[vga_table_index].base_addr=(VGA_TEXT_BUF_ADDR & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;

    // initialize 4KB pages for terminal
    uint32_t terminal_dir_index;
    uint32_t terminal_table_index;
    uint32_t terminal_addr_tmp;
    uint32_t pages;

    for(pages = 0; pages < 3; pages++){
        terminal_addr_tmp = VGA_TERM_0 + 0x1000 * pages;
        terminal_dir_index = dir_entry(terminal_addr_tmp);
        terminal_table_index = page_entry(terminal_addr_tmp);

        page_directory[terminal_dir_index].present=1;
        page_directory[terminal_dir_index].rw=1;
        page_directory[terminal_dir_index].global=1;
        page_directory[terminal_dir_index].base_addr=((uint32_t)page_table0 & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;

        page_table0[terminal_table_index].present=1;
        page_table0[terminal_table_index].rw=1;
        page_table0[terminal_table_index].base_addr=(terminal_addr_tmp & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;
    }

    // Init paging by seting the control registers
    asm volatile(
        "movl $page_directory, %%eax \n\t"          // move page_directory's address to cr3(PBDR)
        "movl %%eax, %%cr3 \n\t"
        "movl %%cr4, %%eax \n\t"
        "orl  $0x10, %%eax \n\t"                    // set 5th in cr4 to 1 to enable mixed size pages
        "movl %%eax, %%cr4 \n\t"
        "movl %%cr0, %%eax \n\t"
        "orl  $0x80000001, %%eax \n\t"              // set the 1th in cr0 and highest bit in cr0 to enable paging
        "movl %%eax, %%cr0 \n\t"
        :   :   : "%eax","cc"    // %eax,cc changed
        );
}

/*
 *   reload_tlb
 *   DESCRIPTION: used to reload TLB
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: will reload the cr3
 */
inline void reload_tlb(){
    asm volatile(
        "movl %%cr3, %%eax \n\t"        // reload cr3 to reload TLB
        "movl %%eax, %%cr3 \n\t"
        :
        :
        : "%eax"
    );
}


int32_t map_4MB_page(uint32_t virtual_addr, uint32_t physical_addr){
    if(virtual_addr==0 || physical_addr==0) return -1;
    uint32_t pd_index = dir_entry(virtual_addr);

    page_directory[pd_index].val = 0;
    page_directory[pd_index].present = 1;
    page_directory[pd_index].rw = 1;
    page_directory[pd_index].us = 1;
    page_directory[pd_index].ps = 1;
    page_directory[pd_index].val = physical_addr>>TABLE_ADDRESS_SHIFT;
    reload_tlb();
    return 0;
}

int32_t map_4KB_page(uint32_t virtual_addr, uint32_t physical_addr){
    if(virtual_addr==0 || physical_addr==0) return -1;
    uint32_t pd_index = dir_entry(virtual_addr);
    uint32_t pt_index = page_entry(virtual_addr);

    page_directory[pd_index].val = 0;
    page_directory[pd_index].present = 1;
    page_directory[pd_index].rw = 1;
    page_directory[pd_index].global=1;
    page_directory[pd_index].base_addr = ((uint32_t)page_table0 & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;

    page_table0[pt_index].present=1;
    page_table0[pt_index].rw=1;
    page_table0[pt_index].base_addr=(physical_addr & ALIGNED_ADDR_MASK)>>TABLE_ADDRESS_SHIFT;
    reload_tlb();
    return 0;
}

