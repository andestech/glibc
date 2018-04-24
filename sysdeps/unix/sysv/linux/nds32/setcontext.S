/* Set current context.
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
        bnez    $r0, 1f

        move    $r0,  $r4
        addi    $r15, $r0, UCONTEXT_GREGS + 4
	lmw.bim	$r1,  [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0,  [$r0 + UCONTEXT_GREGS]
        jr      $r15
1:
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
 
