	.text
	.globl	fred
	.type	fred, @function
fred:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	movslq	a(%rip), %r10
	movslq	b(%rip), %r11
	addq	%r10, %r11
	movslq	c(%rip), %r10
	addq	%r11, %r10
	movslq	d(%rip), %r11
	addq	%r10, %r11
	movslq	e(%rip), %r10
	addq	%r11, %r10
	movslq	f(%rip), %r11
	addq	%r10, %r11
	movslq	g(%rip), %r10
	addq	%r11, %r10
	movl	%r10d, %eax
	jmp	L1
L1:
	addq	$16,%rsp
	popq %rbp
	ret
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	movq	$7, %r11
	pushq	%r11
	movq	$6, %r10
	movq	%r10, %r9
	movq	$5, %r10
	movq	%r10, %r8
	movq	$4, %r10
	movq	%r10, %rcx
	movq	$3, %r10
	movq	%r10, %rdx
	movq	$2, %r10
	movq	%r10, %rsi
	movq	$1, %r10
	movq	%r10, %rdi
	call	fred
	addq	$8, %rsp
	movslq	-8(%rbp), %r11
	movl	%r10d, -8(%rbp)
	movslq	-8(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r10
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L2
L2:
	addq	$16,%rsp
	popq %rbp
	ret
