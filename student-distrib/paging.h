#ifndef PAGING_H
#define PAGING_H

#include "lib.h"
#include "pcb.h"

//the address in table start from the 12th b
#define	TABLE_ADDRESS_SHIFT		12

#define PAGE_DIC_MAX      1024
#define PAGE_TAB_MAX      1024

// Macro for 4KB and 4MB
#define PAGE_4KB_VAL             (1<<12)          // The value of 4MB
#define PAGE_4MB_VAL             (1<<22)          // The value of 4MB
#define PAGE_10BITS_MASK         0x03FF                     // Used to get low 10 bits


// Offset from virtural addr
#define VIRTUAL_DIR_OFF          22                         // offset of page directory in virtual address
#define VIRTUAL_PAG_OFF          12                         // offset of page table in virtual address

#define VALUE_128MB              0x8000000                  // 128 mb
#define VALUE_132MB              0x8400000  

// Addrs
#define KERNEL_ADDR              PAGE_4MB_VAL               // Kernel start at 4MB in physical memory
#define VGA_TEXT_BUF_ADDR        0xB8000                    // VGA text mode starts at 0xB8000

#define VGA_TERM_0 0xB9000
// #define VGA_TERM_1 0xB8A000
// #define VGA_TERM_2 0xB8B000

//#define PROGRAM_START_VIRTUAL_ADDR 0x08048000
#define PROGRAM_START_VIRTUAL_ADDR 0x08048000
#define VIDMAP_START_VIRTUAL_ADDR  0x0C000000
/* if we are to create multiple pages for vga page switching, we would uncomment these lines */

/*
#define VGA_MODEX_ADDR           0x0A0000

#define VGA_TEXT_BUF_ADDR0 (0xB8000)
#define VGA_TEXT_BUF_ADDR1 (0xBA000)
#define VGA_TEXT_BUF_ADDR2 (0xBA000+80*25*2+160)
#define VGA_TEXT_BUF_ADDR3 (0xBA000+80*25*4+160*2)
*/

#define ALIGNED_ADDR_MASK        0xFFFFF000                 // Used to get the 20 LSBs

#define dir_entry(vir_addr)      (((uint32_t)vir_addr>>VIRTUAL_DIR_OFF)&PAGE_10BITS_MASK)   // macro used to get the page directory index from virtual address
#define page_entry(vir_addr)      (((uint32_t)vir_addr>>VIRTUAL_PAG_OFF)&PAGE_10BITS_MASK)   // macro used to get the page table index from virtual address

typedef union PTE{
    uint32_t val;
    struct {
        uint32_t present         :1;    // present flag
        uint32_t rw              :1;    // read and write flag
        uint32_t us              :1;    // user or supervisor flag
        uint32_t pwt             :1;    // pwt flag
        uint32_t pcd             :1;    // pcd flag
        uint32_t accessed        :1;    // accessed flag
        uint32_t dirty           :1;    // accessed flag
        uint32_t ps              :1;    // 4MB or 4KB size flag
        uint32_t global          :1;    // global flag
        uint32_t avl             :3;    // available flag
        uint32_t base_addr       :20;   // base address (4KB aligned)
    } __attribute__ ((packed));
} PTE;

//unit struct in PDE
typedef union PDE{
    uint32_t val;
    struct {
        uint32_t present         :1;    // present flag
        uint32_t rw              :1;    // read and write flag
        uint32_t us              :1;    // user or supervisor flag
        uint32_t pwt             :1;    // pwt flag
        uint32_t pcd             :1;    // pcd flag
        uint32_t accessed        :1;    // accessed flag
        uint32_t dirty           :1;    // dirty flag
        uint32_t ps              :1;    // 4MB or 4KB size flag
        uint32_t global          :1;    // global flag
        uint32_t avl             :3;    // available flag
        uint32_t base_addr       :20;   // base address (4KB aligned)
    } __attribute__ ((packed));
} PDE;

// page_directory
PDE page_directory[PAGE_DIC_MAX] __attribute__((aligned (PAGE_4KB_VAL)));
// first page table
PTE page_table0[PAGE_TAB_MAX] __attribute__((aligned (PAGE_4KB_VAL)));
// second page table for Vidmap
PTE page_table1[PAGE_TAB_MAX] __attribute__((aligned (PAGE_4KB_VAL)));

void init_paging();
inline void reload_tlb();

int32_t map_4MB_page(uint32_t virtual_addr, uint32_t physical_addr);
int32_t map_4KB_page(uint32_t virtual_addr, uint32_t physical_addr);

#endif
