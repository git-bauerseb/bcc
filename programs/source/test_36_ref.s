	.data
	.globl	p
p:	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
L3:
	.byte	37
	.byte	108
	.byte	100
	.byte	10
	.byte	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$0,%rsp
	movq	$42, %r10
	leaq	p(%rip), %r11
	movq	$0, %r12
	addq	%r11, %r12
	movq	%r10, (%r12)
	leaq	p(%rip), %r10
	movq	$0, %r11
	addq	%r10, %r11
	movq	(%r11), %r11
	movq	%r11, %rsi
	leaq	L3(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	movq	%rax, %r10
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L2
L2:
	addq	$0,%rsp
	popq	%rbp
	ret
