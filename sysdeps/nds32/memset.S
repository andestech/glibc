/* Optimized version for Andes nds32.
   Copyright (C) 2006-2018 Free Software Foundation, Inc.

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

#include <sysdep.h>

/* void *memset(void *dst, int val, int len);
 
         dst: $r0
         val: $r1
         len: $r2
         ret: $r0 - pointer to the memory area dst.  */

ENTRY(memset)
	/* Set $r3 as the dest. Keep value of $r0.  */
	move	$r3, $r0
	/* if len<4, go to byte-mode.  */
	slti	$ta, $r2, 4
	bnez	$ta, .Lbyte_mode
	andi	$r5, $r3, 0x3
	beqz	$r5, .Lword_prepare
	/* size to next word-aligned addres.  */
	subri	$r5, $r5, #4
	/* $r2: bytes left.  */
	sub	$r2, $r2, $r5
	add	$r5, $r5, $r0

.align	2
1:
	/* byte-mode set loop.  */
	sbi.bi	$r1, [$r3], 1
	bne	$r3, $r5, 1b

.align  2
.Lword_prepare:
	/* prepare word.  */

	/* $r1  <- 0x000000ab.  */
	andi    $r1, $r1, 0xff
	/* $r4  <- 0x0000ab00.  */
	slli    $r4, $r1, 8
	/* $r1  <- 0x0000abab.  */
	or      $r1, $r1, $r4
	/* $r4  <- 0xabab0000.  */
	slli    $r4, $r1, 16
	/* $r1  <- 0xabababab.  */
	or      $r1, $r1, $r4

	/* assume cache-line size is 32 bytes.  */
	srli	$r5, $r2, 5
	beqz	$r5, .Lword_mode
	slli	$r5, $r5, 5
	add	$r5, $r5, $r3
	andi	$r2, $r2, 0x1f

	/* cache-line-mode set loop.  */
	move $r16,$r1
	move $r17,$r1
	move $r18,$r1
	move $r19,$r1
	move $r20,$r1
	move $r21,$r1
	move $r22,$r1
	move $r23,$r1

.align	2
4:
	/* $r3 is dst.  */
	smw.bim	$r16, [$r3], $r23, #0
	/* Next cache-line.  */
	bne	$r3, $r5, 4b

.align  2
.Lword_mode:
	/* $r5 <- n words to set.  */
	srli	$r5, $r2, 2
	beqz	$r5, .Lbyte_mode
	slli	$r5, $r5, 2
	add	$r5, $r5, $r3
	andi	$r2, $r2, 0x3

	/* word-mode set loop.  */
.align	2
2:
	smw.bim	$r1, [$r3], $r1, #0
	/* Next word.  */
	bne $r3, $r5, 2b

.align  2
.Lbyte_mode:
	beqz	$r2, .Lend
	add	$r5, $r3, $r2

.align	2
3:
	/* byte-mode set loops.  */
	sbi.bi	$r1, [$r3], #1
	bne	$r3, $r5, 3b
.align  2
.Lend:
	ret

END(memset)
libc_hidden_builtin_def (memset)

