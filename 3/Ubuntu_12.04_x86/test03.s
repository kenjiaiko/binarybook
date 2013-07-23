	.file	"test.c"
	.text
.globl get_sp
	.type	get_sp, @function
get_sp:
	pushl	%ebp
	movl	%esp, %ebp
#APP
# 5 "test.c" 1
	movl %esp, %eax
	addl $0x58, %eax
# 0 "" 2
#NO_APP
	popl	%ebp
	ret
	.size	get_sp, .-get_sp
	.section	.rodata
.LC0:
	.string	"0x%08lx"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$64, %esp
	movl	12(%ebp), %eax
	movl	%eax, 28(%esp)
	movl	%gs:20, %eax
	movl	%eax, 60(%esp)
	xorl	%eax, %eax
	call	get_sp
	movl	$.LC0, %edx
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	printf
	call	getchar
	movl	28(%esp), %eax
	addl	$4, %eax
	movl	(%eax), %eax
	movl	%eax, 4(%esp)
	leal	44(%esp), %eax
	movl	%eax, (%esp)
	call	strcpy
	movl	$0, %eax
	movl	60(%esp), %edx
	xorl	%gs:20, %edx
	je	.L5
	call	__stack_chk_fail
.L5:
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	#.section	.note.GNU-stack,"",@progbits
