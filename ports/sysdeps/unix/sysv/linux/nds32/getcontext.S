/* Copyright (C) 20[B01-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Martin Schwidefsky (schwidefsky@de.ibm.com).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <features.h>

#include "ucontext_i.h"

/*  __getcontext (const ucontext_t *ucp)

  Saves the machine context in UCP such that when it is activated,
  it appears as if __getcontext() returned again.

  This implementation is intended to be used for *synchronous* context
  switches only.  Therefore, it does not have to save anything
  other than the PRESERVED state.  */

ENTRY(__getcontext)
        swi     $lp, [$r0 + UCONTEXT_PC]
        addi    $r15, $r0, UCONTEXT_GREGS
        xor     $r1, $r1, $r1
	smw.bim	$r1, [$r15], $r1
	smw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	smw.bim	$r16, [$r15], $r25, #0xf
	move    $r4, $r0

	/* sigprocmask (SIG_BLOCK, NULL, &sc->sc_mask).  */
	move    $r0, SIG_BLOCK
	move	$r1, 0
	lwi	$r2, [$r4 + UCONTEXT_SIGMASK]
	move	$r3, _NSIG8
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, 1f


#ifdef NDS32_ABI_2FP_PLUS
	lwi	$r2, [$r4 + UCONTEXT_FSREGS]
/* Process for FPU registers.  */
	fmfcfg	$r20	/* Keep $fpcfg in $r20.  */
	slli	$r20, $r20, #28
	srli	$r20, $r20, #30	/* Set $r20 as $fpcfg.freg.  */
	swi.bi	$r20, [$r2], #4

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz	$r20, .LCFG0		/* Branch if $fpcfg.freg = 0b00.  */
	xori	$r15, $r20, #0b10
	beqz	$r15, .LCFG2		/* Branch if $fpcfg.freg = 0b10.  */
	srli	$r20, $r20, #0b01
	beqz	$r20, .LCFG1		/* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
	fsdi.bi	$fd31, [$r2], #8
	fsdi.bi	$fd29, [$r2], #8
	fsdi.bi	$fd27, [$r2], #8
	fsdi.bi	$fd25, [$r2], #8
	fsdi.bi	$fd23, [$r2], #8
	fsdi.bi	$fd21, [$r2], #8
	fsdi.bi	$fd19, [$r2], #8
	fsdi.bi	$fd17, [$r2], #8
.LCFG2:
	fsdi.bi	$fd15, [$r2], #8
	fsdi.bi	$fd13, [$r2], #8
	fsdi.bi	$fd11, [$r2], #8
	fsdi.bi	$fd9, [$r2], #8
.LCFG1:
	fsdi.bi	$fd7, [$r2], #8
	fsdi.bi	$fd5, [$r2], #8
.LCFG0:
	fsdi.bi	$fd3, [$r2], #8
#endif /* NDS32_ABI_2FP_PLUS */

	/* Set __getcontext return value to 0.  */
        xor      $r0, $r0, $r0
        /* Return first_return: 1 */
        addi     $r1, $r0, 1
        ret

1:
	move $r0, -1
	ret
END(__getcontext)

weak_alias (__getcontext, getcontext)

