/* Optimized version for Andes nds32.
   Copyright (C) 2013-2018 Free Software Foundation, Inc.

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

/*  Function:
       strcpy - copy a string.
    Syntax:
       char *strcpy(char *dest, const char *src);
    Description:
       This function copies the string pointed to by src into the array
       point to by dest (include the teminating null character).
    Return value:
       strcpy returns the dest as given.  */
#include <sysdep.h>
        .text

ENTRY(strcpy)
	move	$r3, $r0	/* Keep $r0 as reture value.  */
#if defined (__NDS32_EXT_STRING__)
1:
	lmwzb.bm	$r4, [$r1], $r4, #0
	smwzb.bm	$r4, [$r3], $r4, #0
	ffzmism	$r2, $r4, $r4
	beqz	$r2, 1b		/* $r2 = 0, no zero byte exist.  */
	ret $lp

#else /* not (__NDS32_EXT_STRING__) */
	GET_GTABLE ($gp)
	/* If SRC or DEST is unaligned, then copy bytes.  */
	or	$r2, $r1, $r0
	andi	$r2, $r2, #3
	bnez	$r2, .Lbyte_mode

.Lword_mode:
	/* SRC and DEST are both "long int" aligned, try to do "long int"
	   sized copies.  */
	/* #define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
	   DETECTNULL returns nonzero if (long)X contains a NULL byte.  */
	lwi	$r2, [$r1+(0)]		/* $r2 is X.  */
	sethi	$r4, hi20(0xFEFEFEFF)
	ori	$r4, $r4, lo12(0xFEFEFEFF)
	add	$r4, $r2, $r4		/* $r4 = ((X) - 0x01010101).  */
	nor	$r5, $r2, $r2		/* $r5 = ~(X).  */
	and	$r4, $r5, $r4		/* $r4 = ~(X) & ((X) - 0x01010101).  */
	sethi	$r5, hi20(0x80808080)
	ori	$r5, $r5, lo12(0x80808080)
	and	$r4, $r4, $r5		/* $r4 = $r4 & 0x80808080.  */
	bnez	$r4, .Lbyte_mode	/* Contains a NULL byte.  */
	swi.bi	$r2, [$r3], #4
	addi	$r1, $r1, #4
	b	.Lword_mode

.Lbyte_mode:
	lbi.bi	$r4, [$r1], #1		/* $r4  <- *src++.  */
	sbi.bi	$r4, [$r3], #1		/* $r4  -> *dest++.  */
	bnez	$r4, .Lbyte_mode
	ret $lp
#endif /* not (__NDS32_EXT_STRING__) */
END(strcpy)
libc_hidden_builtin_def (strcpy)
