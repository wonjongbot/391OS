# linkage.S - Set up assembly linkage for handlers in IDT
# vim:ts=4 noexpandtab

.text

.globl INT0_linker, INT1_linker, INT2_linker, INT3_linker, INT4_linker, INT5_linker
.globl INT6_linker, INT7_linker, INT8_linker, INT9_linker, INT10_linker, INT11_linker
.globl INT12_linker, INT13_linker, INT14_linker, INT15_linker, INT16_linker, INT17_linker
.globl INT18_linker, INT19_linker, INT20_linker, INT21_linker, INT22_linker, INT23_linker
.globl INT24_linker, INT25_linker, INT26_linker, INT27_linker, INT28_linker, INT29_linker
.globl INT30_linker, INT31_linker, INT80_linker

.align 4

INT0_linker:
PUSHAL
call INT0
POPAL
IRET

INT1_linker:
PUSHAL
call INT1
POPAL
IRET

INT2_linker:
PUSHAL
call INT2
POPAL
IRET

INT3_linker:
PUSHAL
call INT3
POPAL
IRET

INT4_linker:
PUSHAL
call INT4
POPAL
IRET

INT5_linker:
PUSHAL
call INT5
POPAL
IRET

INT6_linker:
PUSHAL
call INT6
POPAL
IRET

INT7_linker:
PUSHAL
call INT7
POPAL
IRET

INT8_linker:
PUSHAL
call INT8
POPAL
IRET

INT9_linker:
PUSHAL
call INT9
POPAL
IRET

INT10_linker:
PUSHAL
call INT10
POPAL
IRET

INT11_linker:
PUSHAL
call INT11
POPAL
IRET

INT12_linker:
PUSHAL
call INT12
POPAL
IRET

INT13_linker:
PUSHAL
call INT13
POPAL
IRET

INT14_linker:
PUSHAL
call INT14
POPAL
IRET

INT15_linker:
PUSHAL
call INT15
POPAL
IRET

INT16_linker:
PUSHAL
call INT16
POPAL
IRET

INT17_linker:
PUSHAL
call INT17
POPAL
IRET

INT18_linker:
PUSHAL
call INT18
POPAL
IRET

INT19_linker:
PUSHAL
call INT19
POPAL
IRET

INT20_linker:
PUSHAL
call INT20
POPAL
IRET

INT21_linker:
PUSHAL
call INT21
POPAL
IRET

INT22_linker:
PUSHAL
call INT22
POPAL
IRET

INT23_linker:
PUSHAL
call INT23
POPAL
IRET

INT24_linker:
PUSHAL
call INT24
POPAL
IRET

INT25_linker:
PUSHAL
call INT25
POPAL
IRET

INT26_linker:
PUSHAL
call INT26
POPAL
IRET

INT27_linker:
PUSHAL
call INT27
POPAL
IRET

INT28_linker:
PUSHAL
call INT28
POPAL
IRET

INT29_linker:
PUSHAL
call INT29
POPAL
IRET

INT30_linker:
PUSHAL
call INT30
POPAL
IRET

INT31_linker:
PUSHAL
call INT31
POPAL
IRET

INT80_linker:
PUSHAL
call INT80
POPAL
IRET


# ctrl + alt + 2 
    # brings up qemu monitor
    # info mem (lists all the paged area)
        # ! if we don't have this properly we ahve PG disabled
    # info registers
    # info pic
