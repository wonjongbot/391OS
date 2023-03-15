#include "lib.h"

// Paging flag define (bit masks)
#define PAGEF_PRESENT            0x1
#define PAGEF_RW                 0x2
#define PAGEF_SIZE               0x80
#define PAGEF_GLOBAL             0x100

// Number of entriy in page directory and page table
#define PAGE_DIC_ENTRY      1024
#define PAGE_TAB_ENTRY      1024

// Macro for 4KB and 4MB
#define PAGE_4KB_OFF             12                         // The bit start of 4KB
#define PAGE_4MB_OFF             22                         // The bit start of 4MB
#define PAGE_4KB_VAL             (1<<PAGE_4KB_OFF)          // The value of 4MB
#define PAGE_4MB_VAL             (1<<PAGE_4MB_OFF)          // The value of 4MB
#define PAGE_10BITS_MASK         0x03FF                     // Used to get low 10 bits
#define PAGE_8MB_OFF             23
#define PAGE_8MB_VAL             (1<<PAGE_8MB_OFF)

// Offset to get the component from virtural addr
#define VIRTUAL_DIR_OFF          22                         // Bits offset of page directory in virtual address
#define VIRTUAL_PAG_OFF          12                         // Bits offset of page table in virtual address
#define ALIGNED_ADDR_OFF         12

// Addrs
#define KERNAL_ADDR              PAGE_4MB_VAL               // Kernal start at 4MB in physical memory
//#define VGA_TEXT_BUF_ADDR        (0xB8000)                   // Text buffer of VGA start at 0xB8000, Referance : https://en.wikipedia.org/wiki/VGA_text_mode - Access methods
#define VGA_MODEX_ADDR           0x0A0000

#define VGA_TEXT_BUF_ADDR (0xB8000)
// #define VGA_TEXT_BUF_ADDR1 (0xB8000+(80*25*2+160))
// #define VGA_TEXT_BUF_ADDR2 (0xB8000+(80*25*2+160)*2)
#define VGA_TEXT_BUF_ADDR1 (0xBA000)
#define VGA_TEXT_BUF_ADDR2 (0xBC000)

#define ALIGNED_ADDR_MASK        0xFFFFF000                 // Used to get the 20 LSBs

#define get_dir_entry(vir_addr)      (((uint32_t)vir_addr>>VIRTUAL_DIR_OFF)&PAGE_10BITS_MASK)   // macro used to get the page directory index from virtual address
#define get_pag_entry(vir_addr)      (((uint32_t)vir_addr>>VIRTUAL_PAG_OFF)&PAGE_10BITS_MASK)   // macro used to get the page table index from virtual address

typedef union PTE{
    uint32_t val;
    struct {
        uint32_t present         :1;    // present flag
        uint32_t rw              :1;    // read and write flag
        uint32_t base_addr       :20;   // base address (4KB aligned)
    } __attribute__ ((packed));
} pte_t;

//unit struct in PDE
typedef union PDE{
    uint32_t val;
    struct {
        uint32_t present         :1;    // present flag
        uint32_t rw              :1;    // read and write flag
        uint32_t base_addr       :20;   // base address (4KB aligned)
    } __attribute__ ((packed));
} pde_t;

// page_directory
pde_t page_directory[PAGE_DIC_ENTRY] __attribute__((aligned (PAGE_4KB_VAL)));
// first page table
pte_t page_table0[PAGE_TAB_ENTRY] __attribute__((aligned (PAGE_4KB_VAL)));
// page table vga
pte_t page_table_vga[PAGE_TAB_ENTRY] __attribute__((aligned (PAGE_4KB_VAL)));
// page table for modex
pte_t page_table_modex[PAGE_TAB_ENTRY] __attribute__((aligned (PAGE_4KB_VAL)));

void init_paging();

