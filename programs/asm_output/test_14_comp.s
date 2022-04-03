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

	.comm	i,1,1
	movq	$1, %r8
	movb	%r8b, i(%rip)
L1:
	movzbq	i(%rip), %r8
	movq	$5, %r9
	cmpq	%r9, %r8
	jg	L2
	movzbq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movzbq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movb	%r9b, i(%rip)
	jmp	L1
L2:
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$1, %r8
	movb	%r8b, i(%rip)
L3:
	movzbq	i(%rip), %r8
	movq	$5, %r9
	cmpq	%r9, %r8
	jg	L4
	movzbq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movzbq	i(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9
	movb	%r9b, i(%rip)
	jmp	L3
L4:
	movl $0, %eax
	popq     %rbp
	ret
