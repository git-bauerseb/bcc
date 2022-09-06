	.data
	.globl	p
p:	.quad	0
L2:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	87
	.byte	111
	.byte	114
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
	leaq	L2(%rip), %r8
	movq	%r8, p(%rip)
L3:
	movq	p(%rip), %r8
	movzbq	(%r8), %r8
	movq	$0, %r9
	cmpq	%r9, %r8
	je	L4
	movq	p(%rip), %r8
	movzbq	(%r8), %r8
	movq	%r8, %rdi
	call	printchar
	movq	%rax, %r9
	movq	p(%rip), %r8
	incq	p(%rip)
	jmp	L3
L4:
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
