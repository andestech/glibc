/* Save and set current context.  Andes nds32 version
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <asm/fp_udfiex_crtl.h>
#include "ucontext_i.h"

/* int swapcontext (ucontext_t *oucp, const ucontext_t *ucp).  */

ENTRY(__swapcontext)
	move	$r5, $r1
        swi     $lp, [$r0 + UCONTEXT_PC]
        addi    $r15, $r0, UCONTEXT_GREGS
        xor     $r1, $r1, $r1
	smw.bim $r1, [$r15], $r1
	addi    $r15, $r15, 20
	smw.bim	$r6, [$r15], $r14
        addi    $r15, $r15, 4
	smw.bim	$r16, [$r15], $r25, #0xf
#ifdef __NDS32_ABI_2FP_PLUS__
	addi	$r4, $r0, UCONTEXT_FDREGS
#endif /* __NDS32_ABI_2FP_PLUS__.  */

/* rt_sigprocmask (SIG_SETMASK, &ucp->uc_sigmask, &oucp->uc_sigmask, _NSIG8).  */
	move	$r3, _NSIG8
	addi	$r2, $r0, UCONTEXT_SIGMASK
	addi	$r1, $r5, UCONTEXT_SIGMASK
	move    $r0, SIG_SETMASK
	movi	$r15, SYS_ify(rt_sigprocmask)
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, .err

#ifdef __NDS32_ABI_2FP_PLUS__
/* Process for FPU registers.  */
	fmfcfg	$r20	/* Keep $fpcfg in $r20.  */
	slli	$r20, $r20, #28
	srli	$r20, $r20, #30	/* Set $r20 as $fpcfg.freg.  */

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz	$r20, .LCFG0		/* Branch if $fpcfg.freg = 0b00.  */
	xori	$r15, $r20, #0b10
	beqz	$r15, .LCFG2		/* Branch if $fpcfg.freg = 0b10.  */
	srli	$r20, $r20, #0b01
	beqz	$r20, .LCFG1		/* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
	fsdi $fd31, [$r4 + 248]
	fsdi $fd30, [$r4 + 240]
	fsdi $fd29, [$r4 + 232]
	fsdi $fd28, [$r4 + 224]
	fsdi $fd27, [$r4 + 216]
	fsdi $fd26, [$r4 + 208]
	fsdi $fd25, [$r4 + 200]
	fsdi $fd24, [$r4 + 192]
.LCFG2:
	fsdi $fd10, [$r4 + 80]
	fsdi $fd9, [$r4 + 72]
	fsdi $fd8, [$r4 + 64]
.LCFG1:
	fsdi $fd7, [$r4 + 56]
	fsdi $fd6, [$r4 + 48]
	fsdi $fd5, [$r4 + 40]
	fsdi $fd4, [$r4 + 32]
.LCFG0:
	fsdi $fd3, [$r4 + 24]
	/* save fpcsr.  */
	fmfcsr $r1
	swi $r1, [$r4 + 0x100]
	/* save the status of udf and iex trap.  */
	movi 	$r0, GET_UDF_IEX_TRAP
	movi	$r15, SYS_ify(fp_udfiex_crtl)
	syscall SYS_ify(fp_udfiex_crtl)
	swi $r0, [$r4 + 0x104]
#endif /* __NDS32_ABI_2FP_PLUS__.  */



#ifdef __NDS32_ABI_2FP_PLUS__
	addi	$r0,  $r5, UCONTEXT_FDREGS
	fmfcfg  $r20    /* Keep $fpcfg in $r20.  */
	slli    $r20, $r20, #28
	srli    $r20, $r20, #30 /* Set $r20 as $fpcfg.freg.  */

	fmfcfg  $r20    /* Keep $fpcfg in $r20.  */
	slli    $r20, $r20, #28
	srli    $r20, $r20, #30 /* Set $r20 as $fpcfg.freg.  */

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz    $r20, .LCFG4         /* Branch if $fpcfg.freg = 0b00.  */
	xori    $r15, $r20, #0b10
	beqz    $r15, .LCFG6         /* Branch if $fpcfg.freg = 0b10.  */
	srli    $r20, $r20, #0b01
	beqz    $r20, .LCFG5         /* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG7:
	fldi $fd31, [$r0 + 248]
	fldi $fd30, [$r0 + 240]
	fldi $fd29, [$r0 + 232]
	fldi $fd28, [$r0 + 224]
	fldi $fd27, [$r0 + 216]
	fldi $fd26, [$r0 + 208]
	fldi $fd25, [$r0 + 200]
	fldi $fd24, [$r0 + 192]
.LCFG6:
	fldi $fd10, [$r0 + 80]
	fldi $fd9, [$r0 + 72]
	fldi $fd8, [$r0 + 64]
.LCFG5:
	fldi $fd7, [$r0 + 56]
	fldi $fd6, [$r0 + 48]
	fldi $fd5, [$r0 + 40]
	fldi $fd4, [$r0 + 32]
.LCFG4:
	fldi $fd3, [$r0 + 24]
	/* save fpcsr.  */
	lwi $r1, [$r0 + 0x100]
	fmtcsr $r1
	/* save the status of udf and iex trap.  */
	lwi 	$r0, [$r0 + 0x104]
	bltz	$r0, 1f
	movi	$r15, SYS_ify(fp_udfiex_crtl)
	syscall SYS_ify(fp_udfiex_crtl)
1:
#endif /* __NDS32_ABI_2FP_PLUS__.  */

        move    $r0, $r5
        addi    $r15, $r5, UCONTEXT_GREGS + 4
	lmw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0, [$r0 + UCONTEXT_GREGS]
        jr      $r15

.err:
	j 	SYSCALL_ERROR_LABEL
PSEUDO_END(__swapcontext)

weak_alias (__swapcontext, swapcontext)
