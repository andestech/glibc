/* Save current context.  Andes LINUX/nds32 version.
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

/*  __getcontext (const ucontext_t *ucp).  */

ENTRY(__getcontext)
        swi     $lp, [$r0 + UCONTEXT_PC]
        addi    $r15, $r0, UCONTEXT_GREGS
        xor     $r1, $r1, $r1
	smw.bim	$r1, [$r15], $r1
        addi    $r15, $r15, 20
	smw.bim	$r6, [$r15], $r14
        addi    $r15, $r15, 4
	smw.bim	$r16, [$r15], $r25, #0xf
#ifdef __NDS32_ABI_2FP_PLUS__
	move	$r4, $r0
#endif

/* rt_sigprocmask (SIG_BLOCK, NULL, &ucp->uc_sigmask, _NSIG8).  */
	move	$r3, _NSIG8
	addi	$r2, $r0, UCONTEXT_SIGMASK
	movi	$r1, 0
	movi    $r0, SIG_BLOCK
	movi	$r15, SYS_ify(rt_sigprocmask)
	syscall 0x0
        bnez    $r0, .err


#ifdef __NDS32_ABI_2FP_PLUS__
	addi	$r2, $r4, UCONTEXT_FDREGS
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
	fsdi $fd31, [$r2 + 248]
	fsdi $fd30, [$r2 + 240]
	fsdi $fd29, [$r2 + 232]
	fsdi $fd28, [$r2 + 224]
	fsdi $fd27, [$r2 + 216]
	fsdi $fd26, [$r2 + 208]
	fsdi $fd25, [$r2 + 200]
	fsdi $fd24, [$r2 + 192]
.LCFG2:
	fsdi $fd10, [$r2 + 80]
	fsdi $fd9, [$r2 + 72]
	fsdi $fd8, [$r2 + 64]
.LCFG1:
	fsdi $fd7, [$r2 + 56]
	fsdi $fd6, [$r2 + 48]
	fsdi $fd5, [$r2 + 40]
	fsdi $fd4, [$r2 + 32]
.LCFG0:
	fsdi $fd3, [$r2 + 24]
	/* save fpcsr.  */
	fmfcsr $r1
	swi $r1, [$r2 + 0x100]
	/* save the status of udf and iex trap.  */
	movi 	$r0, GET_UDF_IEX_TRAP
	movi	$r15, SYS_ify(fp_udfiex_crtl)
	syscall SYS_ify(fp_udfiex_crtl)
	bltz	$r0, .err
	swi $r0, [$r2 + 0x104]
	movi	$r0, 0
#endif /* __NDS32_ABI_2FP_PLUS__.  */
        ret

.err:
	j 	SYSCALL_ERROR_LABEL

PSEUDO_END(__getcontext)

weak_alias (__getcontext, getcontext)

