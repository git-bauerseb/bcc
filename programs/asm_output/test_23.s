	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.long	0
	.data
	.globl	c
c:	.long	0
	.data
	.globl	d
d:	.long	0
	.data
	.globl	e
e:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$2, %r8
	movl	%r8d, a(%rip)
	movq	$4, %r8
	movl	%r8d, b(%rip)
	movq	$3, %r8
	movl	%r8d, c(%rip)
	movq	$2, %r8
	movl	%r8d, d(%rip)
	movzbl	a(%rip), %r8
	movzbl	b(%rip), %r9
	addq	%r8, %r9
	movzbl	c(%rip), %r8
	movzbl	d(%rip), %r10
	addq	%r8, %r10
	imulq	%r9, %r10
	movl	%r10d, e(%rip)
	movzbl	e(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movzbl	a(%rip), %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
