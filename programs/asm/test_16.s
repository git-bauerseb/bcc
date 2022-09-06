	.comm	x, 4, 4
	.comm	y, 4, 4
	.comm	iptr, 8, 8
	movq	$10, %r8
	movl	%r8d, x(%rip)
	movq	$20, %r8
	movl	%r8d, y(%rip)
	leaq	x(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, iptr(%rip)
	movq	iptr(%rip), %r8
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
	movq	$10, %r8
	movl	%r8d, x(%rip)
	movq	$20, %r8
	movl	%r8d, y(%rip)
	leaq	x(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, iptr(%rip)
	movq	iptr(%rip), %r8
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
