	.text
	.globl	param8
	.type	param8, @function
param8:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	addq	$-32, %rsp
	movq	$12, %r10
	movslq	-28(%rbp), %r11
	movl	%r10d, -28(%rbp)
	movslq	-28(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L1
L1:
	addq	$32,%rsp
	popq %rbp
	ret
	.text
	.globl	param5
	.type	param5, @function
param5:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	addq	$-32, %rsp
	movq	$12, %r10
	movslq	-28(%rbp), %r11
	movl	%r10d, -28(%rbp)
	movslq	-28(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L2
L2:
	addq	$32,%rsp
	popq %rbp
	ret
	.text
	.globl	param2
	.type	param2, @function
param2:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	addq	$-32, %rsp
	movq	$12, %r10
	movslq	-28(%rbp), %r11
	movl	%r10d, -28(%rbp)
	movslq	-28(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L3
L3:
	addq	$32,%rsp
	popq %rbp
	ret
	.text
	.globl	param0
	.type	param0, @function
param0:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	addq	$-32, %rsp
	movq	$12, %r10
	movslq	-28(%rbp), %r11
	movl	%r10d, -28(%rbp)
	movslq	-28(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L4
L4:
	addq	$32,%rsp
	popq %rbp
	ret
