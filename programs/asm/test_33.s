	.data
	.data
	.globl	buf
buf:	.quad	0
L6:
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	0
L7:
	.byte	47
	.byte	104
	.byte	111
	.byte	109
	.byte	101
	.byte	47
	.byte	115
	.byte	101
	.byte	98
	.byte	97
	.byte	115
	.byte	116
	.byte	105
	.byte	97
	.byte	110
	.byte	47
	.byte	80
	.byte	114
	.byte	111
	.byte	103
	.byte	114
	.byte	97
	.byte	109
	.byte	109
	.byte	105
	.byte	101
	.byte	114
	.byte	117
	.byte	110
	.byte	103
	.byte	47
	.byte	99
	.byte	47
	.byte	99
	.byte	95
	.byte	99
	.byte	111
	.byte	109
	.byte	112
	.byte	105
	.byte	108
	.byte	101
	.byte	114
	.byte	47
	.byte	112
	.byte	114
	.byte	111
	.byte	103
	.byte	114
	.byte	97
	.byte	109
	.byte	115
	.byte	47
	.byte	115
	.byte	111
	.byte	117
	.byte	114
	.byte	99
	.byte	101
	.byte	47
	.byte	116
	.byte	101
	.byte	115
	.byte	116
	.byte	95
	.byte	51
	.byte	51
	.byte	46
	.byte	112
	.byte	114
	.byte	111
	.byte	103
	.byte	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16, %rsp
	leaq	L6(%rip), %r10
	movq	buf(%rip), %r11
	movq	%r10, buf(%rip)
	movq	$0, %r10
	movq	%r10, %rsi
	leaq	L7(%rip), %r10
	movq	%r10, %rdi
	call	open
	movq	%rax, %r10
	movslq	-4(%rbp), %r11
	movl	%r10d, -4(%rbp)
	movslq	-4(%rbp), %r10
	movq	$1, %r11
	negq	%r11
	cmpq	%r11, %r10
	jne	L8
	movq	$1, %r10
	movl	%r10d, %eax
	jmp	L5
L8:
L9:
	movq	$60, %r10
	movq	%r10, %rdx
	movq	buf(%rip), %r10
	movq	%r10, %rsi
	movslq	-4(%rbp), %r10
	movq	%r10, %rdi
	call	read
	movq	%rax, %r10
	movslq	-8(%rbp), %r11
	movl	%r10d, -8(%rbp)
	movq	$0, %r12
	cmpq	%r12, %r10
	jle	L10
	movslq	-8(%rbp), %r10
	movq	%r10, %rdx
	movq	buf(%rip), %r10
	movq	%r10, %rsi
	movq	$1, %r10
	movq	%r10, %rdi
	call	write
	movq	%rax, %r10
	jmp	L9
L10:
	movslq	-4(%rbp), %r10
	movq	%r10, %rdi
	call	close
	movq	%rax, %r10
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L5
L5:
	addq	$16,%rsp
	popq %rbp
	ret
