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
	addi	$r2, $r4, UCONTEXT_SIGMASK
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
	fsdi $fd31, [$r2 + 248]
	fsdi $fd30, [$r2 + 240]
	fsdi $fd29, [$r2 + 232]
	fsdi $fd28, [$r2 + 224]
	fsdi $fd27, [$r2 + 216]
	fsdi $fd26, [$r2 + 208]
	fsdi $fd25, [$r2 + 200]
	fsdi $fd24, [$r2 + 192]
	fsdi $fd23, [$r2 + 184]
	fsdi $fd22, [$r2 + 176]
	fsdi $fd21, [$r2 + 168]
	fsdi $fd20, [$r2 + 160]
	fsdi $fd19, [$r2 + 152]
	fsdi $fd18, [$r2 + 144]
	fsdi $fd17, [$r2 + 136]
	fsdi $fd16, [$r2 + 128]

.LCFG2:
	fssi $fs31, [$r2 + 124]
	fssi $fs30, [$r2 + 120]
	fssi $fs29, [$r2 + 116]
	fssi $fs28, [$r2 + 112]
	fssi $fs27, [$r2 + 108]
	fssi $fs26, [$r2 + 104]
	fssi $fs25, [$r2 + 100]
	fssi $fs24, [$r2 + 96]
	fssi $fs23, [$r2 + 92]
	fssi $fs22, [$r2 + 88]
	fssi $fs21, [$r2 + 84]
	fssi $fs20, [$r2 + 80]
	fssi $fs19, [$r2 + 76]
	fssi $fs18, [$r2 + 72]
	fssi $fs17, [$r2 + 68]
	fssi $fs16, [$r2 + 64]
.LCFG1:
	fssi $fs15, [$r2 + 60]
	fssi $fs14, [$r2 + 56]
	fssi $fs13, [$r2 + 52]
	fssi $fs12, [$r2 + 48]
	fssi $fs11, [$r2 + 44]
	fssi $fs10, [$r2 + 40]
	fssi $fs9, [$r2 + 36]
	fssi $fs8, [$r2 + 32]
.LCFG0:
	fssi $fs7, [$r2 + 28]
	fssi $fs6, [$r2 + 24]
	fssi $fs5, [$r2 + 20]
	fssi $fs4, [$r2 + 16]
	fssi $fs3, [$r2 + 12]
	fssi $fs2, [$r2 + 8]
	fssi $fs1, [$r2 + 4]
	fssi $fs0, [$r2 + 0]
	/*save fpcsr*/
	fmfcsr $r1
	swi $r1, [$r2 + 0x100]
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

