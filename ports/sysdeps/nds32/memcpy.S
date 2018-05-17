/*
 *  linux/arch/nds32/lib/memcpy.S -- Memory copy function.
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file "COPYING" in the main directory of this archive
 *  for more details.
 *
 *  Copyright (C) 2001  Hiroyuki Kondo, and Hirokazu Takata
 *  Copyright (C) 2004  Hirokazu Takata
 *  Copyright (C) 2009  Andes Technology Corporation
 *
 */


/*
  void *memcpy(void *dst, const void *src, int n);

  dst: $r0
  src: $r1
  n  : $r2
  ret: $r0 - pointer to the memory area dst.
*/


#include <sysdep.h>
	.text

ENTRY(memcpy)
	move    $r3, $r0				! Set $r3 as the dest. Keep value of $r0 .
	! in case not same alignment
	slti	$ta, $r2, 4				! $ta  <- len<4?
	bnez	$ta, .Lbyte_mode		! if len<4, go to byte-mode
	andi	$r4, $r3, 0x3
	andi	$r5, $r1, 0x3

	! handle unligned leading bytes if any
	beqz	$r5, .LWord_aligned		! if word-aligned, go to word-mode
	subri	$r5, $r5, #4			! size to next word-aligned address
	sub		$r2, $r2, $r5			! $r2 <- bytes left
	add		$r5, $r5, $r1			! $r5 <- upper bound

	.align	2
1:
	! byte-mode copy loop
	lbi.bi	$r4, [$r1], 1		! $r4  <- *src++
	sbi.bi	$r4, [$r3], 1		! $r4  -> *dst++
	bne		$r1, $r5, 1b		! if reach upper bound, go to word-mode

    .align  2
.LWord_aligned:
	! assume cache-line size is 32 bytes
	srli	$r5, $r2, 5			! $r5 <- n cache-lines to copy
	beqz	$r5, .Lword_mode	! smaller than cache-line bytes
	slli	$r5, $r5, 5			! $r5 <- n cache-line bytes
	add		$r5, $r5, $r1		! $r5 <- upper bound
	andi	$r2, $r2, 0x1f		! $r2 <- bytes left

	! cache-line-mode copy loop
	.align	2
5:
	lmw.bim	$r16, [$r1], $r23, #0
	smw.bim	$r16, [$r3], $r23, #0
	bne	$r1, $r5, 5b			! Next cache-line

    .align  2
.Lword_mode:
	srli	$r5, $r2, 2			! $r5 <- n words to copy
	beqz	$r5, .Lbyte_mode	! smaller than 4 bytes
	slli	$r5, $r5, 2			! $r5 <- n bytes
	add		$r5, $r5, $r1		! $r5 <- upper bound
	andi	$r2, $r2, 0x3		! $r2 <- bytes left

	! word-mode copy loop
	.align	2
3:
	lmw.bim	$r4, [$r1], $r4, #0
	smw.bim	$r4, [$r3], $r4, #0
	bne		$r1, $r5, 3b		! Next word.

    .align  2
.Lbyte_mode:
	beqz	$r2, .Lend
	add		$r5, $r1, $r2		! Set $r5 as upper bound.

	.align	2
4:
	! byte-mode copy loop
	lbi.bi	$r4, [$r1], #1
	sbi.bi	$r4, [$r3], #1
	bne		$r1, $r5, 4b		! Not readch upper bound. Loop.

    .align  2
.Lend:
	ret

END(memcpy)
libc_hidden_builtin_def (memcpy)

