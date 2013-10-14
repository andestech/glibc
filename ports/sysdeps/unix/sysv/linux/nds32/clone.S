/* Copyright (C) 1996, 1997, 1998, 1999, 2002 Free Software Foundation, Inc.
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

#ifdef PIC
	/* set GP register to parent only, cause child's $SP will be $r1. */
	pushm	$fp, $gp, $sp
#ifndef __NDS32_N1213_43U1H__
	mfusr	$r15, $pc
#endif /* ! __NDS32_N1213_43U1H__  */
	sethi	$gp, hi20(_GLOBAL_OFFSET_TABLE_+4)
	ori	$gp, $gp, lo12(_GLOBAL_OFFSET_TABLE_+8)
	add	$gp, $gp, $r15
#endif /* PIC  */

	/* sanity check arguments.  */
	beqz	$r0, 1f
	bnez	$r1, 2f

1:
	movi	$r0, -EINVAL

5:
#ifdef PIC
	/* restore GP register, only in parent's stack  */
	la	$r15, C_SYMBOL_NAME(__syscall_error@PLT)
	popm	$fp, $gp, $sp
#else /* ! PIC  */
	la	$r15, C_SYMBOL_NAME(__syscall_error)
#endif /* ! PIC  */
	jr	$r15

2:
	/* Child's $sp will be $r1, push to child's stack only.  */
	addi	$r1, $r1, -4
	swi.p	$r3, [$r1], -4			! arg
	swi	$r0, [$r1]			! fn

	/* do the system call  */
	or	$r0, $r2, $r2			! move $r0, $r2
	__do_syscall(clone)
	beqz	$r0, 4f
	bltz	$r0, 5b

	! parent
	! swi     $r0, [$r4]                      ! Store child tid to pd->tid.
#ifdef PIC
	/* restore GP register, only in parent's stack  */
	popm	$fp, $gp, $sp
#endif /* PIC  */
	ret

#ifdef RESET_PID
6:
	swi     $r0, [$r25 + PID_OFFSET]
        swi     $r0, [$r25 + TID_OFFSET]
        j       7f
4:
        beqz    $r3, 6b
	move    $r25, $r3
	addi    $r25, $r25, 0x490               ! TLS_TCB_OFFSET + TLS_PRE_TCB_SIZE
        __do_syscall(getpid)
	swi     $r0, [$r25 + PID_OFFSET]
        swi     $r0, [$r25 + TID_OFFSET]
7:
#else
4:
#endif
	/* Only in child's stack.  */
	pop	$r1				! fn
	pop	$r0				! arg


#if !defined(__NDS32_ABI_2__) && !defined(__NDS32_ABI_2FP__)
	addi	$sp, $sp, -24
#endif /* !defined(__NDS32_ABI_2__) && !defined(__NDS32_ABI_2FP__)  */

	! use $r15 in case _exit is PIC
#ifdef __NDS32_N1213_43U1H__
	or	$r15, $r1, $r1			! move $r15, $r2
#endif /* __NDS32_N1213_43U1H__  */
	bral	$r1

#if !defined(__NDS32_ABI_2__) && !defined(__NDS32_ABI_2FP__)
	addi	$sp, $sp, 24
#endif /* !defined(__NDS32_ABI_2__) && !defined(__NDS32_ABI_2FP__)  */

	! use $r15 in case _exit is PIC
#ifdef PIC
	la	$r15, C_SYMBOL_NAME(_exit@PLT)
#else /* ! PIC  */
	la	$r15, C_SYMBOL_NAME(_exit)
#endif /* ! PIC  */
	jr	$r15


PSEUDO_END (__clone)
weak_alias (__clone, clone)
