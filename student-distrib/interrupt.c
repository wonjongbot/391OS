#include "x86_desc.h"
#include "lib.h"
#include "types.h"

void INT0x00(){
    printf("Interrupt 0:Divide Error Exception (#DE)\n");
    while(1);
}

void INT0x01(){
    printf("Interrupt 1:Debug Exception (#DB)\n");
    while(1);
}

void INT0x02(){
    printf("Interrupt 2:NMI Interrupt\n");
    while(1);
}

void INT0x03(){
    printf("Interrupt 3:Breakpoint Exception (#BP)\n");
    while(1);
}

void INT0x04(){
    printf("Interrupt 4:Overflow Exception (#OF)\n");
    while(1);
}

void INT0x05(){
    printf("Interrupt 5:BOUND Range Exceeded Exception (#BR)\n");
    while(1);
}

void INT0x06(){
    printf("Interrupt 6:Invalid Opcode Exception (#UD)\n");
    while(1);
}

void INT0x07(){
    printf("Interrupt 7:Device Not Available Exception (#NM)\n");
    while(1);
}

void INT0x08(){
    printf("Interrupt 8:Double Fault Exception (#DF)\n");
    while(1);
}

void INT0x09(){
    printf("Interrupt 9:Coprocessor Segment Overrun\n");
    while(1);
}

void INT0x0a(){
    printf("Interrupt 10:Invalid TSS Exception (#TS)\n");
    while(1);
}

void INT0x0b(){
    printf("Interrupt 11:Segment Not Present (#NP)\n");
    while(1);
}

void INT0x0c(){
    printf("Interrupt 12:Stack Fault Exception (#SS)\n");
    while(1);
}

void INT0x0d(){
    printf("Interrupt 13:General Protection Exception (#GP)\n");
    while(1);
}

void INT0x0e(){
    printf("Interrupt 14:Page-Fault Exception (#PF) \n");
    while(1);
}

void INT0x0f(){
    printf("Interrupt 15\n");
    while(1);
}

void INT0x10(){
    printf("Interrupt 16:x87 FPU Floating-Point Error (#MF)\n");
    while(1);
}

void INT0x11(){
    printf("Interrupt 17:Alignment Check Exception (#AC)\n");
    while(1);
}

void INT0x12(){
    printf("Interrupt 18:Machine-Check Exception (#MC)\n");
    while(1);
}

void INT0x13(){
    printf("Interrupt 19:SIMD Floating-Point Exception (#XF)\n");
    while(1);
}

void INT0x14(){
    printf("Interrupt 20\n");
    while(1);
}

void INT0x15(){
    printf("Interrupt 21\n");
    while(1);
}

void INT0x16(){
    printf("Interrupt 22\n");
    while(1);
}

void INT0x17(){
    printf("Interrupt 23\n");
    while(1);
}

void INT0x18(){
    printf("Interrupt 24\n");
    while(1);
}

void INT0x19(){
    printf("Interrupt 25\n");
    while(1);
}

void INT0x1a(){
    printf("Interrupt 26\n");
    while(1);
}

void INT0x1b(){
    printf("Interrupt 27\n");
    while(1);
}

void INT0x1c(){
    printf("Interrupt 28\n");
    while(1);
}

void INT0x1d(){
    printf("Interrupt 29\n");
    while(1);
}

void INT0x1e(){
    printf("Interrupt 30\n");
    while(1);
}

void INT0x1f(){
    printf("Interrupt 31\n");
    while(1);
}

void INT0x21(){
    printf("Interrupt 0x21 -- Keyboard interrupt\n");
}

void INT0x80(){
    printf("SYSTEM CALL 0x80\n");
    while(1);
}

static void init_idt_entry(idt_desc_t desc, uint32_t index, uint32_t DPL, uint32_t handler){
    desc.seg_selector = KERNEL_CS;
    desc.reserved3 = 0x0;
    desc.reserved2 = 0x1;
    desc.reserved1 = 0x1;
    desc.size = 0x1;
    desc.reserved0 = 0x0;
    desc.dpl = 0x3 & DPL;
    desc.present = 0x1;
    SET_IDT_ENTRY(desc, handler);
}

static void init_exception(){
    int i;
    for(i = 0; i < 0x20; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved3 = 0x0;
        idt[i].reserved2 = 0x1;
        idt[i].reserved1 = 0x1;
        idt[i].size = 0x1;
        idt[i].reserved0 = 0x0;
        idt[i].dpl = 0x0;
        idt[i].present=0x1;
    }
}

static void init_interrupt(){
    int i;
    for(i = 0x20; i < 0x30; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved3 = 0x0;
        idt[i].reserved2 = 0x1;
        idt[i].reserved1 = 0x1;
        idt[i].size = 0x1;
        idt[i].reserved0 = 0x0;
        idt[i].dpl = 0x0;
        idt[i].present=0x0;
    }
}

static void init_syscall(){
    int i = 0x80;
    idt[i].seg_selector = KERNEL_CS;
    idt[i].reserved3 = 0x1;
    idt[i].reserved2 = 0x1;
    idt[i].reserved1 = 0x1;
    idt[i].size = 0x1;
    idt[i].reserved0 = 0x0;
    idt[i].dpl = 0x0;
    idt[i].present=0x0;
}

void init_idt(){
    init_exception();
    init_interrupt();
    init_syscall();
    
    // initialize interrupt/trap gates of IDT
    {
    // Interrupt 0:Divide Error Exception (#DE)
    SET_IDT_ENTRY(idt[0], INT0x00_linker);

    // Interrupt 1:Debug Exception (#DB)
    SET_IDT_ENTRY(idt[1], INT0x01_linker);

    // Interrupt 2:NMI Interrupt
    SET_IDT_ENTRY(idt[2], INT0x02_linker);

    // Interrupt 3:Breakpoint Exception (#BP)
    SET_IDT_ENTRY(idt[3], INT0x03_linker);
    
    // Interrupt 4:Overflow Exception (#OF) 
    SET_IDT_ENTRY(idt[4], INT0x04_linker);
    
    // Interrupt 5:BOUND Range Exceeded Exception (#BR) 
    SET_IDT_ENTRY(idt[5], INT0x05_linker);
    
    // Interrupt 6:Invalid Opcode Exception (#UD)
    SET_IDT_ENTRY(idt[6], INT0x06_linker);
    
    // Interrupt 7:Device Not Available Exception (#NM)
    SET_IDT_ENTRY(idt[7], INT0x07_linker);
    
    // Interrupt 8:Double Fault Exception (#DF)
    SET_IDT_ENTRY(idt[8], INT0x08_linker);
    
    // Interrupt 9:Coprocessor Segment Overrun
    SET_IDT_ENTRY(idt[9], INT0x09_linker);
    
    // Interrupt 10:Invalid TSS Exception (#TS)
    SET_IDT_ENTRY(idt[10], INT0x0a_linker);
    
    // Interrupt 11:Segment Not Present (#NP)
    SET_IDT_ENTRY(idt[11], INT0x0b_linker);
    
    // Interrupt 12:Stack Fault Exception (#SS)
    SET_IDT_ENTRY(idt[12], INT0x0c_linker);
    
    // Interrupt 13:General Protection Exception (#GP)
    SET_IDT_ENTRY(idt[13], INT0x0d_linker);
    
    // Interrupt 14:Page-Fault Exception (#PF) 
    SET_IDT_ENTRY(idt[14], INT0x0e_linker);

    // Interrupt 15
    SET_IDT_ENTRY(idt[15], INT0x0f_linker);
    
    // Interrupt 16:x87 FPU Floating-Point Error (#MF)
    SET_IDT_ENTRY(idt[16], INT0x10_linker);
    
    // Interrupt 17:Alignment Check Exception (#AC)
    SET_IDT_ENTRY(idt[17], INT0x11_linker);
    
    // Interrupt 18:Machine-Check Exception (#MC)
    SET_IDT_ENTRY(idt[18], INT0x12_linker);
    
    // Interrupt 19:SIMD Floating-Point Exception (#XF)
    SET_IDT_ENTRY(idt[19], INT0x13_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[20], INT0x14_linker);

    // Interrupt 21
    SET_IDT_ENTRY(idt[21], INT0x15_linker);

    // Interrupt 22
    SET_IDT_ENTRY(idt[22], INT0x16_linker);

    // Interrupt 23
    SET_IDT_ENTRY(idt[23], INT0x17_linker);
    
    // Interrupt 24
    SET_IDT_ENTRY(idt[24], INT0x18_linker);

    // Interrupt 25
    SET_IDT_ENTRY(idt[25], INT0x19_linker);

    // Interrupt 26
    SET_IDT_ENTRY(idt[26], INT0x1a_linker);
    
    // Interrupt 27
    SET_IDT_ENTRY(idt[27], INT0x1b_linker);

    // Interrupt 28
    SET_IDT_ENTRY(idt[28], INT0x1c_linker);

    // Interrupt 29
    SET_IDT_ENTRY(idt[29], INT0x1d_linker);

    // Interrupt 30
    SET_IDT_ENTRY(idt[30], INT0x1e_linker);

    // Interrupt 31
    SET_IDT_ENTRY(idt[31], INT0x1f_linker);

    // Interrupt 0x21 -- keyboard
    SET_IDT_ENTRY(idt[0x21], INT0x21_linker);

    // system call
    SET_IDT_ENTRY(idt[0x80], INT0x80_linker);
    }

    // set lidt
    lidt(idt_desc_ptr);
}
