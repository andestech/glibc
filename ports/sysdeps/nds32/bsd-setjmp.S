/* BSD `setjmp' entry point to `sigsetjmp (..., 1)'.  NDS version.
   Copyright (C) 1996, 1997, 2000 Free Software Foundation, Inc.
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

/* This just does a tail-call to `__sigsetjmp (ARG, 1)'.
   We cannot do it in C because it must be a tail-call, so frame-unwinding
   in setjmp doesn't clobber the state restored by longjmp.  */

#include <sysdep.h>

ENTRY(setjmp)
	move	$r1, #1

/* Make a tail call to __sigsetjmp.  */
#ifdef PIC
	/* Initialize $r2 as $gp value.  */
	sethi   $r2, hi20(_GLOBAL_OFFSET_TABLE_-8)
	ori     $r2, $r2, lo12(_GLOBAL_OFFSET_TABLE_-4)
	mfusr   $r15, $pc
	add     $r2, $r15, $r2

	! la    $r3, __sigsetjmp@PLT
	sethi   $r3, hi20(HIDDEN_JUMPTARGET(__sigsetjmp)@PLT)
	ori     $r3, $r3, lo12(HIDDEN_JUMPTARGET(__sigsetjmp)@PLT)
	add     $r3, $r3, $r2

	jr      $r3
#else /* NOT PIC */
	la	$r15, C_SYMBOL_NAME(__sigsetjmp)
	jr	$r15
#endif

END(setjmp)
