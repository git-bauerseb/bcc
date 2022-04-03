	movq	$20, %r8
	movl	%r8d, %eax
	jmp	L1
	.text
	.globl	fred
	.type	fred, @function
fred:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$20, %r9
	movl	%r9d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
	.comm	result, 4, 4
	movq	$15, %r8
	movq	%r8, %rdi
	call	fred
	movq	%rax, %r9
	movl	%r9d, result(%rip)
	movzbl	result(%rip), %r8
	movq	$10, %r9
	addq	%r8, %r9
	movq	%r9, %rdi
	call	printint
	movq	%rax, %r8
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L2
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$15, %r8
	movq	%r8, %rdi
	call	fred
	movq	%rax, %r9
	movl	%r9d, result(%rip)
	movzbl	result(%rip), %r8
	movq	$10, %r9
	addq	%r8, %r9
	movq	%r9, %rdi
	call	printint
	movq	%rax, %r8
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L2
L2:
	popq %rbp
	ret
