	.text
	.file	"foo.c"
	.globl	_Z3fooi                         # -- Begin function _Z3fooi
	.p2align	4, 0x90
	.type	_Z3fooi,@function
_Z3fooi:                                # @_Z3fooi
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$2, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB0_2
# %bb.1:
	jmp	.LBB0_4
.LBB0_2:
	jmp	.LBB0_3
.LBB0_3:
	imull	$3, -4(%rbp), %eax
	movl	%eax, -4(%rbp)
.LBB0_4:
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	_Z3fooi, .Lfunc_end0-_Z3fooi
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 11.0.0"
	.section	".note.GNU-stack","",@progbits
	.addrsig
