#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "interrupt.h"
#include "linkage.h"

void INT0(){
    printf("Interrupt 0:Divide Error Exception (#DE)\n");
    while(1);
}

void INT1(){
    printf("Interrupt 1:Debug Exception (#DB)\n");
    while(1);
}

void INT2(){
    printf("Interrupt 2:NMI Interrupt\n");
    while(1);
}

void INT3(){
    printf("Interrupt 3:Breakpoint Exception (#BP)\n");
    while(1);
}

void INT4(){
    printf("Interrupt 4:Overflow Exception (#OF)\n");
    while(1);
}

void INT5(){
    printf("Interrupt 5:BOUND Range Exceeded Exception (#BR)\n");
    while(1);
}

void INT6(){
    printf("Interrupt 6:Invalid Opcode Exception (#UD)\n");
    while(1);
}

void INT7(){
    printf("Interrupt 7:Device Not Available Exception (#NM)\n");
    while(1);
}

void INT8(){
    printf("Interrupt 8:Double Fault Exception (#DF)\n");
    while(1);
}

void INT9(){
    printf("Interrupt 9:Coprocessor Segment Overrun\n");
    while(1);
}

void INT10(){
    printf("Interrupt 10:Invalid TSS Exception (#TS)\n");
    while(1);
}

void INT11(){
    printf("Interrupt 11:Segment Not Present (#NP)\n");
    while(1);
}

void INT12(){
    printf("Interrupt 12:Stack Fault Exception (#SS)\n");
    while(1);
}

void INT13(){
    printf("Interrupt 13:General Protection Exception (#GP)\n");
    while(1);
}

void INT14(){
    printf("Interrupt 14:Page-Fault Exception (#PF) \n");
    while(1);
}

void INT15(){
    printf("Interrupt 15\n");
    while(1);
}

void INT16(){
    printf("Interrupt 16:x87 FPU Floating-Point Error (#MF)\n");
    while(1);
}

void INT17(){
    printf("Interrupt 17:Alignment Check Exception (#AC)\n");
    while(1);
}

void INT18(){
    printf("Interrupt 18:Machine-Check Exception (#MC)\n");
    while(1);
}

void INT19(){
    printf("Interrupt 19:SIMD Floating-Point Exception (#XF)\n");
    while(1);
}

void INT20(){
    printf("Interrupt 20\n");
    while(1);
}

void INT21(){
    printf("Interrupt 21\n");
    while(1);
}

void INT22(){
    printf("Interrupt 22\n");
    while(1);
}

void INT23(){
    printf("Interrupt 23\n");
    while(1);
}

void INT24(){
    printf("Interrupt 24\n");
    while(1);
}

void INT25(){
    printf("Interrupt 25\n");
    while(1);
}

void INT26(){
    printf("Interrupt 26\n");
    while(1);
}

void INT27(){
    printf("Interrupt 27\n");
    while(1);
}

void INT28(){
    printf("Interrupt 28\n");
    while(1);
}

void INT29(){
    printf("Interrupt 29\n");
    while(1);
}

void INT30(){
    printf("Interrupt 30\n");
    while(1);
}

void INT31(){
    printf("Interrupt 31\n");
    while(1);
}

void INT80(){
    printf("SYSTEM CALL 0x80\n");
    while(1);
}

void init_idt_entry(idt_desc_t desc, uint32_t index, uint32_t DPL, uint32_t handler){
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

void init_exception(){
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

void init_interrupt(){
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

void init_syscall(){
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
    SET_IDT_ENTRY(idt[0], INT0_linker);

    // Interrupt 1:Debug Exception (#DB)
    SET_IDT_ENTRY(idt[1], INT1_linker);

    // Interrupt 2:NMI Interrupt
    SET_IDT_ENTRY(idt[2], INT2_linker);

    // Interrupt 3:Breakpoint Exception (#BP)
    SET_IDT_ENTRY(idt[3], INT3_linker);
    
    // Interrupt 4:Overflow Exception (#OF) 
    SET_IDT_ENTRY(idt[4], INT4_linker);
    
    // Interrupt 5:BOUND Range Exceeded Exception (#BR) 
    SET_IDT_ENTRY(idt[5], INT5_linker);
    
    // Interrupt 6:Invalid Opcode Exception (#UD)
    SET_IDT_ENTRY(idt[6], INT6_linker);
    
    // Interrupt 7:Device Not Available Exception (#NM)
    SET_IDT_ENTRY(idt[7], INT7_linker);
    
    // Interrupt 8:Double Fault Exception (#DF)
    SET_IDT_ENTRY(idt[8], INT8_linker);
    
    // Interrupt 9:Coprocessor Segment Overrun
    SET_IDT_ENTRY(idt[9], INT9_linker);
    
    // Interrupt 10:Invalid TSS Exception (#TS)
    SET_IDT_ENTRY(idt[10], INT10_linker);
    
    // Interrupt 11:Segment Not Present (#NP)
    SET_IDT_ENTRY(idt[11], INT11_linker);
    
    // Interrupt 12:Stack Fault Exception (#SS)
    SET_IDT_ENTRY(idt[12], INT12_linker);
    
    // Interrupt 13:General Protection Exception (#GP)
    SET_IDT_ENTRY(idt[13], INT13_linker);
    
    // Interrupt 14:Page-Fault Exception (#PF) 
    SET_IDT_ENTRY(idt[14], INT14_linker);

    // Interrupt 15
    SET_IDT_ENTRY(idt[15], INT15_linker);
    
    // Interrupt 16:x87 FPU Floating-Point Error (#MF)
    SET_IDT_ENTRY(idt[16], INT16_linker);
    
    // Interrupt 17:Alignment Check Exception (#AC)
    SET_IDT_ENTRY(idt[17], INT17_linker);
    
    // Interrupt 18:Machine-Check Exception (#MC)
    SET_IDT_ENTRY(idt[18], INT18_linker);
    
    // Interrupt 19:SIMD Floating-Point Exception (#XF)
    SET_IDT_ENTRY(idt[19], INT19_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[20], INT20_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[21], INT21_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[22], INT22_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[23], INT23_linker);
    
    // Interrupt 20
    SET_IDT_ENTRY(idt[24], INT24_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[25], INT25_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[26], INT26_linker);
    
    // Interrupt 20
    SET_IDT_ENTRY(idt[27], INT27_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[28], INT28_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[29], INT29_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[30], INT30_linker);

    // Interrupt 20
    SET_IDT_ENTRY(idt[31], INT31_linker);

    // system call
    SET_IDT_ENTRY(idt[0x80], INT80_linker);
    }

    // set lidt
    lidt(idt_desc_ptr);
}
