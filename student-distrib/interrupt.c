#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "linkage.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "system_calls.h"

// set_atrib function takes in 4 bits for foreground color and 4 bits for background color
// colors we used are :
// 		- 0x07: black background with grey text
//		- 0x0B: black background with cyan text
//		- 0x02: black background with green text
//		- 0x4E: red background with yellow text

/*
 * INT0x00
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x00
 */
void INT0x00(){
    set_attrib(0x4E);
    printf("[!] Interrupt 0:Divide Error Exception (#DE)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT0x01
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x01
 */
void INT0x01(){
    set_attrib(0x4E);
    printf("[!] Interrupt 1:Debug Exception (#DB)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x02()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x02
 */
void INT0x02(){
    set_attrib(0x4E);
    printf("[!] Interrupt 2:NMI Interrupt\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x03()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x03
 */
void INT0x03(){
    set_attrib(0x4E);
    printf("[!] Interrupt 3:Breakpoint Exception (#BP)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x04()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x04
 */
void INT0x04(){
    set_attrib(0x4E);
    printf("[!] Interrupt 4:Overflow Exception (#OF)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x05()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x05
 */
void INT0x05(){
    set_attrib(0x4E);
    printf("[!] Interrupt 5:BOUND Range Exceeded Exception (#BR)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x06()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x06
 */
void INT0x06(){
    set_attrib(0x4E);
    printf("[!] Interrupt 6:Invalid Opcode Exception (#UD)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x07()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x07
 */
void INT0x07(){
    set_attrib(0x4E);
    printf("[!] Interrupt 7:Device Not Available Exception (#NM)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x08()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x08
 */
void INT0x08(){
    set_attrib(0x4E);
    printf("[!] Interrupt 8:Double Fault Exception (#DF)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x09()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x09
 */
void INT0x09(){
    set_attrib(0x4E);
    printf("[!] Interrupt 9:Coprocessor Segment Overrun\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x0a()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0A
 */
void INT0x0a(){
    set_attrib(0x4E);
    printf("[!] Interrupt 10:Invalid TSS Exception (#TS)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x0b()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0b
 */
void INT0x0b(){
    set_attrib(0x4E);
    printf("[!] Interrupt 11:Segment Not Present (#NP)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x0c()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0c
 */
void INT0x0c(){
    set_attrib(0x4E);
    printf("[!] Interrupt 12:Stack Fault Exception (#SS)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x0d()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0d
 */
void INT0x0d(){
    set_attrib(0x4E);
    printf("[!] Interrupt 13:General Protection Exception (#GP)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x0e()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0e
 */
void INT0x0e(){
    set_attrib(0x4E);
    printf("[!] Interrupt 14:Page-Fault Exception (#PF) \n");
    set_attrib(0x7);
//    while(1);
    syscall_halt(-0x0e);
}

/*
 * INT_0x0f()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x0f
 */
void INT0x0f(){
    set_attrib(0x4E);
    printf("[!] Interrupt 15\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x10()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x10
 */
void INT0x10(){
    set_attrib(0x4E);
    printf("[!] Interrupt 16:x87 FPU Floating-Point Error (#MF)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x11()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x11
 */
void INT0x11(){
    set_attrib(0x4E);
    printf("[!] Interrupt 17:Alignment Check Exception (#AC)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x12()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x12
 */
void INT0x12(){
    set_attrib(0x4E);
    printf("[!] Interrupt 18:Machine-Check Exception (#MC)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x13()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x13
 */
void INT0x13(){
    set_attrib(0x4E);
    printf("[!] Interrupt 19:SIMD Floating-Point Exception (#XF)\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x14()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x14
 */
void INT0x14(){
    set_attrib(0x4E);
    printf("[!] Interrupt 20\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x15()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x15
 */
void INT0x15(){
    set_attrib(0x4E);
    printf("[!] Interrupt 21\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x16()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x16
 */
void INT0x16(){
    set_attrib(0x4E);
    printf("[!] Interrupt 22\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x17()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x17
 */
void INT0x17(){
    set_attrib(0x4E);
    printf("[!] Interrupt 23\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x18()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x18
 */
void INT0x18(){
    set_attrib(0x4E);
    printf("[!] Interrupt 24\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x19()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x19
 */
void INT0x19(){
    set_attrib(0x4E);
    printf("[!] Interrupt 25\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1a()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1a
 */
void INT0x1a(){
    set_attrib(0x4E);
    printf("[!] Interrupt 26\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1b()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1b
 */
void INT0x1b(){
    set_attrib(0x4E);
    printf("[!] Interrupt 27\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1c()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1c
 */
void INT0x1c(){
    set_attrib(0x4E);
    printf("[!] Interrupt 28\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1d()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1d
 */
void INT0x1d(){
    set_attrib(0x4E);
    printf("[!] Interrupt 29\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1e()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1e
 */
void INT0x1e(){
    set_attrib(0x4E);
    printf("[!] Interrupt 30\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x1f()
 * Input: None
 * Output: None
 * Interrupt handler for exception 0x1f
 */
void INT0x1f(){
    set_attrib(0x4E);
    printf("[!] Interrupt 31\n");
    set_attrib(0x7);
    while(1);
}

/*
 * INT_0x21()
 * Input: None
 * Output: None
 * Interrupt handler for keyboard interrupt
 */
void INT0x21(){
    keyboard_handler();
}

/*
 * INT_0x28()
 * Input: None
 * Output: None
 * Interrupt handler for rtc interrupt
 */
void INT0x28(){
    rtc_handler();
}

/*
 * INT_0x80()
 * Input: None
 * Output: None
 * Interrupt handler for system calls
 */
void INT0x80(){
    printf("SYSTEM CALL 0x80\n");
    while(1);
}

/*
 * init_exception()
 * Input: None
 * Output: None
 * Initializes IDT entries 0-31 for exceptions
 */
static void init_exception(){
    int i;
    for(i = 0; i < 0x20; i++){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved3 = 0x1;
        idt[i].reserved2 = 0x1;
        idt[i].reserved1 = 0x1;
        idt[i].size = 0x1;
        idt[i].reserved0 = 0x0;
        idt[i].dpl = 0x0;
        idt[i].present=0x1;
    }
}

/*
 * init_interrupt()
 * Input: None
 * Output: None
 * Initializes IDT entries 32-47 for interrupts
 */
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
        idt[i].present=0x1;
    }
}

/*
 * init_syscall()
 * Input: None
 * Output: None
 * Initializes IDT entry 128 for syscall
 */
static void init_syscall(){
    int i = 0x80;
    idt[i].seg_selector = KERNEL_CS;
    idt[i].reserved3 = 0x1;
    idt[i].reserved2 = 0x1;
    idt[i].reserved1 = 0x1;
    idt[i].size = 0x1;
    idt[i].reserved0 = 0x0;
    idt[i].dpl = 0x3;
    idt[i].present=0x1;
}

/*
 *   set_system_gate
 *   DESCRIPTION: use to set the system gate in IDT
 *   INPUTS: n - the index of IDT entry
 *           addr - the address of handler function
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: will change the idt array
 */
void set_system_gate(int n, void* addr){
    // Call SET_IDT_ENTRY macro to init offset
    SET_IDT_ENTRY(idt[n], addr);

}


/*
 * init_idt()
 * Input: None
 * Output: None
 * Initializes and sets values of the IDT table for exceptions, interrupts, and the syscall
 */
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

    // Interrupt 0x28 -- rtc
    SET_IDT_ENTRY(idt[0x28], INT0x28_linker);

    // system call
    SET_IDT_ENTRY(idt[0x80], SYSTEM_CALL_HANDLER);
    }

    // Init System Call in IDT (vec 0x80 is system call)
    //set_system_gate(0x80, SYSTEM_CALL_HANDLER);

    // set lidt
    lidt(idt_desc_ptr);
}
