	.comm	c, 4, 4
	.comm	e, 8, 8
	.comm	f, 8, 8
	movq	$42, %r8
	movl	%r8d, c(%rip)
	leaq	c(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, e(%rip)
	movq	e(%rip), %r8
	movq	$1, %r9
	subq	%r9, %r8
	movq	%r8, f(%rip)
	movq	f(%rip), %r8
	movq	(%r8), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$42, %r8
	movl	%r8d, c(%rip)
	leaq	c(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, e(%rip)
	movq	e(%rip), %r8
	movq	$1, %r9
	subq	%r9, %r8
	movq	%r8, f(%rip)
	movq	f(%rip), %r8
	movq	(%r8), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
