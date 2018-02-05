#include <sysdep.h>
#include <libc-symbols.h>

ENTRY(_dl_runtime_resolve)

	! we get called with
	!  lp contains the return address from this call
	!  r16 contains offset to target reloc entry
	!  r17 contains GOT[1] (identity of taget lib)
	!  r15 is GOT[2] (starting address of _dl_runtime_resolve)

	! save arguments r0 - r3
	smw.adm $r0,	[$sp],	$r5,	6
	.cfi_adjust_cfa_offset 32
	.cfi_rel_offset lp, 28 
	.cfi_rel_offset gp, 24

        sethi   $gp,    HI20(_GLOBAL_OFFSET_TABLE_-8)
        ori     $gp,    $gp,    LO12(_GLOBAL_OFFSET_TABLE_-4)
	mfusr 	$r15,	$pc
        add     $gp,    $r15,    $gp

	move	$r0,	$r17
	! sizeof (ElfW(Rela)) is 12
	slli    $r1,    $r16,   2
	slli    $r16,   $r16,   3
	add     $r1,    $r1,    $r16
	addi	$r2,	$lp,	0
	bal	_dl_fixup
	! save the return
	addi	$r15,	$r0,	0
	
	lmw.bim	$r0,	[$sp],	$r5,	6
	.cfi_adjust_cfa_offset -32
	.cfi_restore lp
	.cfi_restore gp

	! jump to the newly found address
	jr		$r15
END(_dl_runtime_resolve)
ENTRY(_dl_runtime_profile)

	! we get called with
	!   lp contains the return address from this call
	!   r16 contains offset to target reloc entry
	!   r17 contains GOT[1] (identity of taget lib)
	!   r15 is GOT[2] (starting address of this function)

	! Stack layout:
	!   12 - La_nds32_regs (9 registers)
	!   4 - Saved two arguments to _dl_profile_fixup
	!   0 - framesize returned from pltenter

	! save La_nds32_regs arguments: r0 - r5, gp, lp, sp
	move 	$r15,	$sp
	smw.adm $r0,	[$r15],	$r5,	7
	move	$sp,	$r15
	.cfi_adjust_cfa_offset 36
	.cfi_rel_offset gp, 24
	.cfi_rel_offset lp, 28

        sethi   $gp,    HI20(_GLOBAL_OFFSET_TABLE_-8)
        ori     $gp,    $gp,    LO12(_GLOBAL_OFFSET_TABLE_-4)
	mfusr 	$r15,	$pc
        add     $gp,    $r15,    $gp

	addi	$r0,	$r17,	0
	slli	$r1,	$r16,	2
	slli	$r16,	$r16,	3
	add	$r1,	$r1,	$r16

	smw.adm $r0,    [$sp],  $r1,    0
	.cfi_adjust_cfa_offset 4

	xor	$r4,	$r4,	$r4
	push	$r4

	move	$r2,	$lp
	addi	$r3,	$sp,	12
	move	$r4,	$sp

	bal	_dl_profile_fixup
	lw	$r2,	[$sp]
	sw	$r0,	[$sp]
	bgez	$r2,	1f
	cfi_remember_state

	addi	$r15,	$sp,	12
	lmw.bim	$r0,	[$r15],	$r5,	7
	.cfi_adjust_cfa_offset -48
	.cfi_restore gp
	.cfi_restore lp
	lwi	$r15,	[$sp + (-48)]

	! jump to the newly found address
	jr	$r15
1:
	! The new frame size is in $r2

	! New stack layout
	!   16 - La_nds32_regs (9 registers)
	!   8 - Saved two arguments to _dl_profile_fixup
	!   4 - Saved result of _dl_profile_fixup
	!   0 - saved $r6

	cfi_restore_state
	push	$r6

	! Make new frame size 8-byte aligned
	bitci	$r6,	$r2,	7


	! Copy stack argument
	sub	$r0,	$sp,	$r6
	addi	$r1,	$sp,	48
	bal	memcpy

	! jump to the newly found address
	addi	$r15,	$sp,	16
	lmw.bim	$r0,	[$r15],	$r5,	0
	lwi	$r15,	[$sp +4]
	sub	$sp,	$sp,	$r6
	jral	$r15
	add	$sp,	$sp,	$r6
	pop	$r6	
	cfi_def_cfa_register (sp)
	! New stack layout
	!   24 - La_nds32_regs (9 registers)
	!   16 - Saved two arguments to _dl_profile_fixup
	!   12 - Saved result of _dl_profile_fixup
	!   8 - one dummy word for stack aligned
	!   0 - La_nds32_retval
	
	! push $r0, $r1 for La_nds32_retval
	! push $r2 for dummy word
	smw.adm	$r0,	[$sp],	$r2
	.cfi_adjust_cfa_offset 12
	addi	$r15,	$sp,	16
	lmw.bim $r0,    [$r15],  $r1	
	addi	$r2,	$sp,	24
	! $r3 outregs
	move	$r3,	$sp
	bal	_dl_call_pltexit

	lmw.bim $r0,    [$sp],  $r1
	addi	$r15,	$sp,	24
	lmw.bim	$r2,	[$r15],	$r5,	7
	.cfi_adjust_cfa_offset -48
	.cfi_restore gp
	.cfi_restore lp
	ret
END(_dl_runtime_profile)
