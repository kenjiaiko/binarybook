	.file	"sample4.c"
	.text
	.p2align 4,,15
.globl func
	.type	func, @function
func:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	leave
	ret
	.size	func, .-func
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$12, %esp
	movl	$3, 8(%esp)
	movl	$2, 4(%esp)
	movl	$1, (%esp)
	call	func
	movl	$0, %eax
	addl	$12, %esp
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.2.2 20070831 prerelease [FreeBSD]"
