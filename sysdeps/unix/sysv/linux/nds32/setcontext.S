/* Set current context.  Andes nds32 version.
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

#include "ucontext_i.h"

/* int __setcontext (const ucontext_t *ucp).  */

ENTRY(__setcontext)
        move    $r4, $r0

/* sigprocmask (SIG_SETMASK, &ucp->uc_sigmask, NULL, _NSIG8).  */
	move    $r0, SIG_SETMASK
	addi	$r1, $r4, UCONTEXT_SIGMASK
	movi	$r2, 0
	movi	$r3, _NSIG8
	movi	$r15, SYS_ify(rt_sigprocmask)
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, .err

#ifdef __NDS32_ABI_2FP_PLUS__
        addi    $r3,  $r4, UCONTEXT_FDREGS

        fmfcfg  $r20    /* Keep $fpcfg in $r20.  */
        slli    $r20, $r20, #28
        srli    $r20, $r20, #30 /* Set $r20 as $fpcfg.freg.  */


        /* Case switch for $r20 as $fpcfg.freg.  */
        beqz    $r20, .LCFG0         /* Branch if $fpcfg.freg = 0b00.  */
        xori    $r15, $r20, #0b10
        beqz    $r15, .LCFG2         /* Branch if $fpcfg.freg = 0b10.  */
        srli    $r20, $r20, #0b01
        beqz    $r20, .LCFG1         /* Branch if $fpcfg.freg = 0b01.  */
        /* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
        fldi $fd31, [$r3 + 248]
        fldi $fd30, [$r3 + 240]
        fldi $fd29, [$r3 + 232]
        fldi $fd28, [$r3 + 224]
        fldi $fd27, [$r3 + 216]
        fldi $fd26, [$r3 + 208]
        fldi $fd25, [$r3 + 200]
        fldi $fd24, [$r3 + 192]
.LCFG2:
        fldi $fd10, [$r3 + 80]
        fldi $fd9, [$r3 + 72]
        fldi $fd8, [$r3 + 64]
.LCFG1:
        fldi $fd7, [$r3 + 56]
        fldi $fd6, [$r3 + 48]
        fldi $fd5, [$r3 + 40]
        fldi $fd4, [$r3 + 32]
.LCFG0:
        fldi $fd3, [$r3 + 24]
        /* save fpcsr.  */
        lwi $r1, [$r3 + 0x100]
        fmtcsr $r1
	/* save the status of udf and iex trap.  */
	lwi 	$r0, [$r3 + 0x104]
	bltz	$r0, 1f
	movi	$r15, SYS_ify(fp_udfiex_crtl)
	syscall SYS_ify(fp_udfiex_crtl)
1:
#endif /* __NDS32_ABI_2FP_PLUS__.  */

        move    $r0,  $r4
        addi    $r15, $r0, UCONTEXT_GREGS + 4
	lmw.bim	$r1,  [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0,  [$r0 + UCONTEXT_GREGS]
        jr      $r15
.err:
	j 	SYSCALL_ERROR_LABEL
PSEUDO_END(__setcontext)

weak_alias (__setcontext, setcontext)

        cfi_startproc
        cfi_undefined (lp)
        nop16
        cfi_endproc
ENTRY (__startcontext)
	beqz $r6, 1f
        move $r0, $r6
	jal __setcontext
1:	
	move $r0, 0
	j HIDDEN_JUMPTARGET(exit)
END (__startcontext)

.hidden __startcontext
 
