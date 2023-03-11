#ifndef _INTERRUPT_H
#define _INTERRUPT_H

extern void init_idt();

/* interrupt handlers */
extern void INT0();
extern void INT1();
extern void INT2();
extern void INT3();
extern void INT4();
extern void INT5();
extern void INT6();
extern void INT7();
extern void INT8();
extern void INT9();
extern void INT10();
extern void INT11();
extern void INT12();
extern void INT13();
extern void INT14();
extern void INT15();
extern void INT16();
extern void INT17();
extern void INT18();
extern void INT19();
extern void INT20();
extern void INT21();
extern void INT22();
extern void INT23();
extern void INT24();
extern void INT25();
extern void INT26();
extern void INT27();
extern void INT28();
extern void INT29();
extern void INT30();
extern void INT31();
extern void INT80();

/* linker to handlers */
extern void INT0_linker();
extern void INT1_linker();
extern void INT2_linker();
extern void INT3_linker();
extern void INT4_linker();
extern void INT5_linker();
extern void INT6_linker();
extern void INT7_linker();
extern void INT8_linker();
extern void INT9_linker();
extern void INT10_linker();
extern void INT11_linker();
extern void INT12_linker();
extern void INT13_linker();
extern void INT14_linker();
extern void INT15_linker();
extern void INT16_linker();
extern void INT17_linker();
extern void INT18_linker();
extern void INT19_linker();
extern void INT20_linker();
extern void INT21_linker();
extern void INT22_linker();
extern void INT23_linker();
extern void INT24_linker();
extern void INT25_linker();
extern void INT26_linker();
extern void INT27_linker();
extern void INT28_linker();
extern void INT29_linker();
extern void INT30_linker();
extern void INT31_linker();
extern void INT80_linker();

#endif
