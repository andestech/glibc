/* setjmp for Andes nds32.
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
#ifdef PIC
# define JMP_BY_R2(symbol)						\
	sethi   $r3, hi20(symbol@PLT);					\
	ori     $r3, $r3, lo12(symbol@PLT);				\
	add     $r3, $r3, $r2;						\
	jr      $r3
# define TCALL_SIGJMP_SAVE						\
	GET_GTABLE ($r2);						\
	JMP_BY_R2 (__sigjmp_save)
#else
# define JMP_BY_R2(symbol)						\
	la	$r15, C_SYMBOL_NAME(symbol);				\
	jr	$r15
# define TCALL_SIGJMP_SAVE						\
	JMP_BY_R2 (__sigjmp_save)
#endif

ENTRY(_setjmp)
	move	$r1, #0
	/* Initialize $r2 as $gp value.  */
	GET_GTABLE ($r2)
        /* Make a tail call to __sigsetjmp.  */
	JMP_BY_R2 (HIDDEN_JUMPTARGET (__sigsetjmp))
END(_setjmp)
ENTRY(setjmp)
	move	$r1, #1
	/* Initialize $r2 as $gp value.  */
	GET_GTABLE ($r2)
        /* Make a tail call to __sigsetjmp.  */
	JMP_BY_R2 (HIDDEN_JUMPTARGET (__sigsetjmp))
END(setjmp)

ENTRY(__sigsetjmp)
	move	$r2, $r0
	/* Save callee-saved registers into buffer.  */
	smw.bim	$r6, [$r2], $r14, #0xf
        /* Make a tail call to __sigjmp_save.  */
	TCALL_SIGJMP_SAVE

END(__sigsetjmp)
hidden_def(__sigsetjmp)
weak_alias (_setjmp, __GI__setjmp)
