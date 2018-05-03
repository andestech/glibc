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
	move	$r5, $r0		/* Return value.  */
	beqz    $r2, .Lend_memset	/* Exit when len = 0.  */
	srli	$r3, $r2, 2		/* $r3 is how many words to copy.  */
	andi	$r2, $r2, 3		/* How many bytes are less than a word.  */
	beqz	$r3, .Lbyte_set		/* When n is less than a word.  */

	/* set $r1 from ??????ab to abababab.  */
	andi    $r1, $r1, 0x00ff	/* $r1 = 000000ab.  */
	slli    $r4, $r1, 8		/* $r4 = 0000ab00.  */
	or      $r1, $r1, $r4		/* $r1 = 0000abab.  */
	slli    $r4, $r1, 16		/* $r4 = abab0000.  */
	or      $r1, $r1, $r4		/* $r1 = abababab.  */

.Lword_set:
	addi	$r3, $r3, -1		/* How many words left to copy.  */
	smw.bim $r1, [$r5], $r1		/* Copy the word to det.  */
	bnez    $r3, .Lword_set		/* Still words to set, continue looping.  */
	beqz	$r2, .Lend_memset	/* No left byte to set.  */

.Lbyte_set:				/* Less than 4 bytes left to set.  */
	addi	$r2, $r2, -1		/* Decrease len by 1.  */
	sbi.p	$r1, [$r5], 1		/* Set data of the next byte to $r1.  */
	bnez	$r2, .Lbyte_set		/* Still bytes left to set.  */

.Lend_memset:
	ret

END(memset)
libc_hidden_builtin_def (memset)

