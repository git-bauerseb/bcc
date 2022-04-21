	.data
	.globl	str
str:	.quad	0
L2:
	.byte	84
	.byte	72
	.byte	73
	.byte	83
	.byte	32
	.byte	73
	.byte	83
	.byte	32
	.byte	65
	.byte	32
	.byte	84
	.byte	69
	.byte	83
	.byte	84
	.byte	10
	.byte	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	leaq	L2(%rip), %r8
	movq	%r8, str(%rip)
L3:
	movq	str(%rip), %r8
	movzbq	(%r8), %r8
	movq	$0, %r9
	cmpq	%r9, %r8
	je	L4
	movq	str(%rip), %r8
	movzbq	(%r8), %r8
	movq	%r8, %rdi
	call	printchar
	movq	%rax, %r9
	movq	str(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, str(%rip)
	jmp	L3
L4:
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq %rbp
	ret
