# linkage.S - Set up assembly linkage for handlers in IDT
# vim:ts=4 noexpandtab

/* This macro generates comman asm linkage of interrupt handlers */
#define CREATE_LINKER(name, handler)     \
    .globl name                         ;\
    name:                               ;\
        pushal                          ;\
        call handler                    ;\
        popal                           ;\
        iret

/* This macro generates comman asm linkage of interrupt handlers that push error code on stack*/
#define CREATE_LINKER_poparg(name, handler)     \
    .globl name                         ;\
    name:                               ;\
        pushal                          ;\
        call handler                    ;\
        popal                           ;\
        addl $4, %esp                   ;\
        iret

/* linkage wrappers */
CREATE_LINKER(INT0x00_linker, INT0x00)
CREATE_LINKER(INT0x01_linker, INT0x01)
CREATE_LINKER(INT0x02_linker, INT0x02)
CREATE_LINKER(INT0x03_linker, INT0x03)
CREATE_LINKER(INT0x04_linker, INT0x04)
CREATE_LINKER(INT0x05_linker, INT0x05)
CREATE_LINKER(INT0x06_linker, INT0x06)
CREATE_LINKER(INT0x07_linker, INT0x07)
CREATE_LINKER_poparg(INT0x08_linker, INT0x08)
CREATE_LINKER(INT0x09_linker, INT0x09)
CREATE_LINKER(INT0x0a_linker, INT0x0a)
CREATE_LINKER(INT0x0b_linker, INT0x0b)
CREATE_LINKER(INT0x0c_linker, INT0x0c)
CREATE_LINKER(INT0x0d_linker, INT0x0d)
CREATE_LINKER(INT0x0e_linker, INT0x0e)
CREATE_LINKER(INT0x0f_linker, INT0x0f)
CREATE_LINKER_poparg(INT0x10_linker, INT0x10)
CREATE_LINKER_poparg(INT0x11_linker, INT0x11)
CREATE_LINKER_poparg(INT0x12_linker, INT0x12)
CREATE_LINKER_poparg(INT0x13_linker, INT0x13)
CREATE_LINKER_poparg(INT0x14_linker, INT0x14)
CREATE_LINKER(INT0x15_linker, INT0x15)
CREATE_LINKER(INT0x16_linker, INT0x16)
CREATE_LINKER_poparg(INT0x17_linker, INT0x17)
CREATE_LINKER(INT0x18_linker, INT0x18)
CREATE_LINKER(INT0x19_linker, INT0x19)
CREATE_LINKER(INT0x1a_linker, INT0x1a)
CREATE_LINKER(INT0x1b_linker, INT0x1b)
CREATE_LINKER(INT0x1c_linker, INT0x1c)
CREATE_LINKER(INT0x1d_linker, INT0x1d)
CREATE_LINKER(INT0x1e_linker, INT0x1e)
CREATE_LINKER(INT0x1f_linker, INT0x1f)
CREATE_LINKER(INT0x21_linker, INT0x21)
CREATE_LINKER(INT0x28_linker, INT0x28)
CREATE_LINKER(INT0x80_linker, INT0x80)

// # ctrl + alt + 2 
//     # brings up qemu monitor
//     # info mem (lists all the paged area)
//         # ! if we don't have this properly we ahve PG disabled
//     # info registers
//     # info pic
