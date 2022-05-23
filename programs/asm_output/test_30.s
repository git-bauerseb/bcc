	.data
	.data
	.globl	a
a:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$0, %rsp
	movq	$10, %r8
	movslq	-4(%rbp), %r9
	movl	%r8d, -4(%rbp)
	movq	$2, %r8
	movslq	a(%rip), %r9
	movl	%r8d, a(%rip)
	movslq	-4(%rbp), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	addq	$0,%rsp
	popq %rbp
	ret
