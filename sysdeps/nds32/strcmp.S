/* Optimized version for Andes nds32.
   Copyright (C) 2015-2018 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/*
    Function:
       strcmp - compare two strings.
    Syntax:
       int strcmp(const char *s1, const char *s2);
    Description:
       The strcmp function compares the string pointed to by s1 to the string
       pointed to by s2.
    Return value:
       Return n integer greater than, equal to, or less than zero, accordingly
       as the string pointed to by s1 is greater than, equal to, or less than
       the string pointed to by s2.
*/
#include <sysdep.h>
        .text

ENTRY(strcmp)
#if defined (__NDS32_EXT_STRING__)
1:
	lmwzb.bm	$r2, [$r0], $r2, #0
	lmwzb.bm	$r3, [$r1], $r3, #0
	ffzmism	$r4, $r2, $r3
	beqz	$r4, 1b
	lb	$r2, [$r0+$r4]
	lb	$r3, [$r1+$r4]
	sub	$r0, $r2, $r3
	ret $lp
#else /* not (__NDS32_EXT_STRING__) */
	/* If s1 or s2 are unaligned, then compare bytes.  */
	or	$r5, $r1, $r0
	andi    $r5, $r5, #3
	bnez    $r5, .Lbyte_mode

	/* If s1 and s2 are word-aligned, compare them a word at a time.  */
	lwi	$r5, [$r0+(0)]
	lwi	$r3, [$r1+(0)]
.off_16bit	/* Dedicate 4-byte to eliminate NOP caused by ".align 2".  */
	bne	$r5, $r3, .Lbyte_mode	/* A difference was detected, so
					   search bytewise. */
.restore_16bit


	/* It's more efficient to set bit mask outside the word_mode loop.  */
	sethi	$r4, hi20(0xFEFEFEFF)	/* Set $r4 as -0x01010101.  */
	ori	$r4, $r4, lo12(0xFEFEFEFF)
	sethi	$r2, hi20(0x80808080)	/* Set $r2 as 0x80808080.  */
	ori	$r2, $r2, lo12(0x80808080)

.align	2
.Lword_mode_loop:
	/* #define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
	   DETECTNULL returns nonzero if (long)X contains a NULL byte.  */

/* Force 32-bit instruction to gain better performance.  */
.off_16bit
	nor	$r3, $r5, $r5		/* $r3 = ~(X).  */
	add	$r5, $r5, $r4		/* $r5 = ((X) - 0x01010101).  */
	and	$r5, $r5, $r3		/* $r5 = ~(X) & ((X) - 0x01010101).  */
	and	$r5, $r5, $r2		/* $r5= r5 & 0x80808080.  */
.restore_16bit

	bnez	$r5, .Lfound_null	/* Branch if $r5 is nonzero.  */
	lmw.aim	$r5, [$r0], $r5, 0
	lmw.aim	$r3, [$r1], $r3, 0
	beq	$r5, $r3, .Lword_mode_loop

.align	2
.Lbyte_mode:
	/* Byte-mode compare.  */
	lbi.bi	$r5, [$r0], #1
	lbi.bi	$r3, [$r1], #1
	bne	$r5, $r3, 1f	/* Mismatch, done.  */
	bnez	$r5, .Lbyte_mode
1:
	sub $r0, $r5, $r3
	ret $lp

.align	2
.Lfound_null:
	/* To get here, *a1 == *a2, thus if we find a null in *a1,
	   then the strings must be equal, so return zero.  */
	movi	$r0, #0
	ret $lp

#endif /* not (__NDS32_EXT_STRING__) */
END(strcmp)
libc_hidden_builtin_def (strcmp)
