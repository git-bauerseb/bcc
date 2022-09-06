	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$42, %r8
	movl	%r8d, a(%rip)
	movslq	a(%rip), %r8
	decl	a(%rip)
	movl	%r8d, b(%rip)
	movslq	a(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
