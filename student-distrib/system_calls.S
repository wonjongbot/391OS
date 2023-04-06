#define ASM 1
.globl SYSTEM_CALL_HANDLER


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


system_call_jumptable:
    .long 0x0, halt, execute, read, write, open, close

