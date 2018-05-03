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

/* void *memcpy(void *dst, const void *src, int n);

   dst: $r0
   src: $r1
   n  : $r2
   ret: $r0 - pointer to the memory area dst.  */


#include <sysdep.h>
	.text

ENTRY(memcpy)
	beq	$r0, $r1, .Lquit_memcpy2
	beqz	$r2, .Lquit_memcpy2
	move	$r5, $r0
	srli    $r3, $r2, #5	/* Check if len < cache-line size 32.  */
	beqz	$r3, .Lword_copy_entry
	andi	$r4, $r0, #0x3	/* Check byte-align.  */
	beqz	$r4, .Lunalign_word_copy_entry

	addi	$r4, $r4,#-4
	abs	$r4, $r4	/* Check how many un-align byte to copy.  */
	sub	$r2, $r2, $r4	/* Update $r2.  */

.Lunalign_byte_copy:
	lbi.bi	$r3, [$r1], #1
	addi	$r4, $r4, #-1
	sbi.bi	$r3, [$r0], #1
	bnez	$r4, .Lunalign_byte_copy
	beqz	$r2, .Lquit_memcpy

.Lunalign_word_copy_entry:
	andi	$r3, $r0, 0x1f	/* Check cache-line unaligncount.  */
	beqz	$r3, .Lcache_copy

	addi	$r3, $r3, #-32
	abs	$r3, $r3
	sub	$r2, $r2, $r3	/* Update $r2.  */

.Lunalign_word_copy:
	lmw.bim	$r4, [$r1], $r4
	addi	$r3, $r3, #-4
	smw.bim	$r4, [$r0], $r4
	bnez	$r3, .Lunalign_word_copy
	beqz	$r2, .Lquit_memcpy
	
	addi	$r3, $r2, #-32	/* To check $r2 < cache_line, than go to .Lword_copy.  */
	bltz	$r3, .Lword_copy_entry
.Lcache_copy:
	srli	$r3, $r2, #5
	beqz	$r3, .Lword_copy_entry
	pushm	$r6, $r13
	.cfi_adjust_cfa_offset 32
	.cfi_rel_offset r6, 0
	.cfi_rel_offset r7, 4
	.cfi_rel_offset r8, 8
	.cfi_rel_offset r9, 12
	.cfi_rel_offset r10, 16
	.cfi_rel_offset r11, 20
	.cfi_rel_offset r12, 24
	.cfi_rel_offset r13, 28
3:
	lmw.bim	$r6, [$r1], $r13
	addi	$r3, $r3, #-1
	smw.bim	$r6, [$r0], $r13
	bnez	$r3, 3b
	popm	$r6, $r13
	.cfi_adjust_cfa_offset -32
	.cfi_restore r6
	.cfi_restore r7
	.cfi_restore r8
	.cfi_restore r9
	.cfi_restore r10
	.cfi_restore r11
	.cfi_restore r12
	.cfi_restore r13

.Lword_copy_entry:
	andi	$r2, $r2, #31

	beqz	$r2, .Lquit_memcpy 
	srli	$r3, $r2, #2
	beqz	$r3, .Lbyte_copy
.Lword_copy:
	lmw.bim	$r4, [$r1], $r4
	addi	$r3, $r3, #-1
	smw.bim	$r4, [$r0], $r4
	bnez	$r3, .Lword_copy
	andi	$r2, $r2, #3
	beqz	$r2, .Lquit_memcpy
.Lbyte_copy:
	lbi.bi	$r3, [$r1], #1
	addi	$r2, $r2, #-1

	sbi.bi	$r3, [$r0], #1
	bnez	$r2, .Lbyte_copy
.Lquit_memcpy:
	move	$r0, $r5
.Lquit_memcpy2:
	ret

END(memcpy)
libc_hidden_builtin_def (memcpy)

