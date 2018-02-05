/* Copyright (C) 2010-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Pat Beirne <patb@corelcomputer.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#include <sysdep.h>
#define _ERRNO_H	1
#include <bits/errno.h>

#ifdef RESET_PID
#include <tcb-offsets.h>
#endif
	
/* int clone(int (*fn)(void *), void *child_stack, int flags, void *arg);
   _syscall2(int, clone, int, flags, void *, child_stack)  */

ENTRY(__clone)
#ifdef __NDS32_ABI_2FP_PLUS__
	lwi	$r4, [$sp]
	lwi	$r5, [$sp+4]
#endif
	/* sanity check arguments.  */
	beqz	$r0, 1f
	bnez	$r1, 2f

1:
	movi	$r0, -EINVAL

5:
	j	SYSCALL_ERROR_LABEL

2:
	/* Child's $sp will be $r1, push to child's stack only.  */
	addi	$r1, $r1, -4
	swi.p	$r3, [$r1], -4			! arg
	swi	$r0, [$r1]			! fn

	/* do the system call  */
	or	$r0, $r2, $r2			! move $r0, $r2

	move    $r3, $r5
	move    $r2, $r4

#ifdef __NDS32_ABI_2FP_PLUS__
       lwi     $r4, [$sp+#0x8]
#else
       lwi     $r4, [$sp]
#endif

	__do_syscall(clone)
	beqz    $r0, 4f
	bltz    $r0, 5b


10:
	ret

4:
	.cfi_undefined lp
	/* Only in child's stack.  */
	pop	$r1				! fn
	pop	$r0				! arg

	bral	$r1

	__do_syscall(exit)

PSEUDO_END (__clone)
libc_hidden_def (__clone)
weak_alias (__clone, clone)
