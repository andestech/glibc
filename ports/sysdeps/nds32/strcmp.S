/*
Copyright (c) 2015 Andes Technology Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    The name of the company may not be used to endorse or promote
    products derived from this software without specific prior written
    permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL RED HAT INCORPORATED BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
	nor	$r3, $r5, $r5		/* r3 = ~(X).  */
	add	$r5, $r5, $r4		/* r5 = ((X) - 0x01010101).  */
	and	$r5, $r5, $r3		/* r5 = ~(X) & ((X) - 0x01010101).  */
	and	$r5, $r5, $r2		/* r5= r5 & 0x80808080.  */
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
