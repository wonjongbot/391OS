#define ASM 1
#include "x86_desc.h"

.text
.globl SYSTEM_CALL_HANDLER
system_call_jumptable:
    .long 0x0, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close


SYSTEM_CALL_HANDLER:
    # need to protect other registers
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %esp
    pushfl

    pushl %edx
    pushl %ecx
    pushl %ebx

    cmpl $1, %eax   # System Call 0 doesnt exist
    jb FAIL
    cmpl $10, %eax  # System Call numbers go up to 10
    ja FAIL

    call *system_call_jumptable(, %eax, 4)

    jmp FINISH


FAIL:
    movel $-1, %eax     # -1 is returned for error

FINISH:
    popl %ebx
    popl %ecx
    popl %edx
    
    popfl
    popl %esp
    popl %ebp
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx

    iret

# wrapper function (User Level)
halt:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl $1, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret

execute:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl $2, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret
    
read:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl 12(%esp), %ecx
    movl 16(%esp), %edx
    movl $3, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret
    
write:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl 12(%esp), %ecx
    movl 16(%esp), %edx
    movl $4, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret
    
open:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl $5, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret

close:
    pushl %ebx      # callee saved
    movl 8(%esp), %ebx      # move argument to the registers
    movl $6, %eax
    int $0x80       # access to the kernel
    popl %ebx
    ret




