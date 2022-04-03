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

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	.comm	i,8,8
	.comm	j,8,8
	.comm	k,8,8
	.comm	l,8,8
	.comm	m,8,8
	movq	$0, %r8
	movq	%r8, i(%rip)
	movq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, j(%rip)
	movq	j(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, k(%rip)
	movq	k(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movq	%r9, l(%rip)
	movq	l(%rip), %r8
	movq	$2, %r9
	addq	%r8, %r9
	movq	%r9, m(%rip)
	movq	m(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
