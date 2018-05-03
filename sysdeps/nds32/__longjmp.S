/* longjmp, Andes nds32 version.
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

#include <sysdep.h>
#define _SETJMP_H
#define _ASM
#include <bits/setjmp.h>
	.section .text

ENTRY(__longjmp)
	/* Restore registers.  */
	lmw.bim  $r6, [$r0], $r14, #0xf

	/* Return error code. Make sure error code is not 0.  */
	bnez  $r1, .Ldone
	movi  $r1, #1
.Ldone:
	addi  $r0, $r1, #0
	ret
END(__longjmp)
