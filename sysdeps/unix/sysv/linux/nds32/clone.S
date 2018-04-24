/* Wrapper around clone system call, Andes nds32 version.
   Copyright (C) 1996-2018 Free Software Foundation, Inc.
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

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#include <sysdep.h>
#define _ERRNO_H	1
#include <bits/errno.h>

/* int clone(int (*fn)(void *), void *child_stack, int flags, void *arg,
	    void *parent_tidptr, void *tls, void *child_tidptr).  */

ENTRY(__clone)
	/* Sanity check arguments.  */
	beqz	$r0, 1f
	bnez	$r1, 2f
1:
	movi	$r0, -EINVAL
5:
	j	SYSCALL_ERROR_LABEL
2:
	/* Child's $sp will be $r1, push to child's stack only.  */
	addi	$r1, $r1, -4
	swi.p	$r3, [$r1], -4
	swi	$r0, [$r1]

	/* Do the system call.  */
	move	$r0, $r2
	move    $r3, $r5
	move    $r2, $r4
        lwi     $r4, [$sp]
	__do_syscall (clone)

	beqz    $r0, 4f
	bltz    $r0, 5b
	ret
4:
	.cfi_undefined lp
        /* Restore the arg for user's function.  */
	pop	$r1	/* Function pointer.  */
	pop	$r0	/* Argument pointer.  */

        /* Call the user's function.  */
	bral	$r1

	__do_syscall (exit)

PSEUDO_END (__clone)
libc_hidden_def (__clone)
weak_alias (__clone, clone)
