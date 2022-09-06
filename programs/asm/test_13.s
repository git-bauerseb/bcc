	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.comm	k,8,8
	.comm	l,8,8
	movq	$1, %r8
	movq	%r8, l(%rip)
	movq	$1, %r8
	movq	%r8, k(%rip)
L1:
	movq	k(%rip), %r8
	movq	$13, %r9
	cmpq	%r9, %r8
	jge	L2
	movq	l(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	l(%rip), %r8
	movq	$2, %r9
	imulq	%r8, %r9
	movq	%r9, l(%rip)
	movq	k(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, k(%rip)
	jmp	L1
L2:
	.text
	.globl	some_function
	.type	some_function, @function
some_function:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$1, %r8
	movq	%r8, l(%rip)
	movq	$1, %r8
	movq	%r8, k(%rip)
L3:
	movq	k(%rip), %r8
	movq	$13, %r9
	cmpq	%r9, %r8
	jge	L4
	movq	l(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	l(%rip), %r8
	movq	$2, %r9
	imulq	%r8, %r9
	movq	%r9, l(%rip)
	movq	k(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, k(%rip)
	jmp	L3
L4:
	movl $0, %eax
	popq     %rbp
	ret
	.comm	i,8,8
	movq	$1, %r8
	movq	%r8, i(%rip)
L5:
	movq	i(%rip), %r8
	movq	$10, %r9
	cmpq	%r9, %r8
	jg	L6
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L5
L6:
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$1, %r8
	movq	%r8, i(%rip)
L7:
	movq	i(%rip), %r8
	movq	$10, %r9
	cmpq	%r9, %r8
	jg	L8
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L7
L8:
	movl $0, %eax
	popq     %rbp
	ret
