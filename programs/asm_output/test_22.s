	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.quad	0
	.data
	.globl	c
c:	.byte	0
	.data
	.globl	d
d:	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	leaq	a(%rip), %r8
	movq	%r8, b(%rip)
	movq	$15, %r8
	movq	b(%rip), %r9
	movq	%r8, (%r9)
	movzbl	a(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	leaq	c(%rip), %r8
	movq	%r8, d(%rip)
	movq	$16, %r8
	movq	d(%rip), %r9
	movb	%r8b, (%r9)
	movzbq	c(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
