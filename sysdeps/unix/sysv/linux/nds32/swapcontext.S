/* Save and set current context.
   Copyright (C) 2009-2018 Free Software Foundation, Inc.
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

/* rt_sigprocmask (SIG_SETMASK, &ucp->uc_sigmask, &oucp->uc_sigmask, _NSIG8).  */
	move	$r3, _NSIG8
	addi	$r2, $r0, UCONTEXT_SIGMASK
	addi	$r1, $r5, UCONTEXT_SIGMASK
	move    $r0, SIG_SETMASK
	movi	$r15, SYS_ify(rt_sigprocmask)
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, 1f

        move    $r0, $r5
        addi    $r15, $r5, UCONTEXT_GREGS + 4
	lmw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0, [$r0 + UCONTEXT_GREGS]
        jr      $r15

1:
	j 	SYSCALL_ERROR_LABEL
PSEUDO_END(__swapcontext)

weak_alias (__swapcontext, swapcontext)
