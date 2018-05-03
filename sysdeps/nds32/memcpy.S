/* Memory copy, Andes nds32 version
   Copyright (C) 2009-2018 Free Software Foundation, Inc.

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

/* void *memcpy(void *dst, const void *src, int n);

   dst: $r0
   src: $r1
   n  : $r2
   ret: $r0 - pointer to the memory area dst.  */

ENTRY (memcpy)
	/* Set $r3 as the dest. Keep value of $r0.  */
	move    $r3, $r0
	/* if len<4, go to byte-mode.  */
	slti	$ta, $r2, 4
	bnez	$ta, .Lbyte_mode
	andi	$r4, $r3, 0x3
	andi	$r5, $r1, 0x3

	/* handle unligned leading bytes.  */
	beqz	$r5, .LWord_aligned
	subri	$r5, $r5, #4
	sub	$r2, $r2, $r5
	add	$r5, $r5, $r1

.align	2
1:
	/* byte-mode copy loop.  */
	lbi.bi	$r4, [$r1], 1
	sbi.bi	$r4, [$r3], 1
	bne	$r1, $r5, 1b

.align  2
.LWord_aligned:
	/* assume cache-line size is 32 bytes.  */
	srli	$r5, $r2, 5
	beqz	$r5, .Lword_mode
	slli	$r5, $r5, 5
	add	$r5, $r5, $r1
	andi	$r2, $r2, 0x1f

	/* cache-line-mode copy loop.  */
.align	2
5:
	lmw.bim	$r16, [$r1], $r23, #0
	smw.bim	$r16, [$r3], $r23, #0
	/* Next cache-line.  */
	bne	$r1, $r5, 5b

.align  2
.Lword_mode:
	srli	$r5, $r2, 2
	beqz	$r5, .Lbyte_mode
	slli	$r5, $r5, 2
	add	$r5, $r5, $r1
	andi	$r2, $r2, 0x3

	/* word-mode copy loop.  */
.align	2
3:
	lmw.bim	$r4, [$r1], $r4, #0
	smw.bim	$r4, [$r3], $r4, #0
	/* Next word.  */
	bne	$r1, $r5, 3b

.align  2
.Lbyte_mode:
	beqz	$r2, .Lend
	add	$r5, $r1, $r2

.align	2
4:
	/* byte-mode copy loop.  */
	lbi.bi	$r4, [$r1], #1
	sbi.bi	$r4, [$r3], #1
	bne	$r1, $r5, 4b

.align  2
.Lend:
	ret

END (memcpy)
libc_hidden_builtin_def (memcpy)

