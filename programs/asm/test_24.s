	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$42, %r8
	leaq	b(%rip), %r9
	movq	$1, %r10
	salq	$2, %r10
	addq	%r9, %r10
	movq	%r8, (%r10)
	leaq	b(%rip), %r8
	movq	$1, %r9
	salq	$2, %r9
	addq	%r8, %r9
	movq	(%r9), %r9
	movq	%r9, %rdi
	call	printint
	movq	%rax, %r8
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
