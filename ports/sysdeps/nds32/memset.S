/* Copyright (C) 1998, 2003 Free Software Foundation, Inc.   This file is part of the GNU C Library.
   Contributed by Philip Blundell <philb@gnu.org>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

   History:

        Sep.06.2006     Created by Harry.
                        Initial porting for NDS32 by Eric.
  */

#include <sysdep.h>

!==========================================================
!  void *memset(void *dst, int val, int len);
!
!        dst: $r0
!        val: $r1
!        len: $r2
!        ret: $r0 - pointer to the memory area dst.
!==========================================================
ENTRY(memset)
	move	$r3, $r0			! Set $r3 as the dest. Keep value of $r0.
								! $r0 <- dst, $r1 <- value, $r2 <- size
	slti	$ta, $r2, 4			! $ta  <- len<4?
	bnez	$ta, .Lbyte_mode	! if len<4, go to byte-mode
	andi	$r5, $r3, 0x3

	! handle unligned leading bytes if any
	beqz	$r5, .Lword_prepare	! if word-aligned, go to word-mode
	subri	$r5, $r5, #4		! size to next word-aligned address
	sub		$r2, $r2, $r5		! $r2 <- bytes left
	add		$r5, $r5, $r0		! $r5 <- upper bound

	.align	2
1:
	! byte-mode set loop
	sbi.bi	$r1, [$r3], 1		! $r1  -> *dst++
	bne		$r3, $r5, 1b		! if reach upper bound, go to word-mode

    .align  2
.Lword_prepare:
	! prepare word
	andi    $r1, $r1, 0xff		! $r1  <- 0x000000ab
	slli    $r4, $r1, 8		! $r4  <- 0x0000ab00
	or      $r1, $r1, $r4		! $r1  <- 0x0000abab
	slli    $r4, $r1, 16		! $r4  <- 0xabab0000
	or      $r1, $r1, $r4		! $r1  <- 0xabababab

	! assume cache-line size is 32 bytes
	srli	$r5, $r2, 5			! $r5 <- n cache-lines to set
	beqz	$r5, .Lword_mode	! smaller than cache-line bytes
	slli	$r5, $r5, 5			! $r5 <- n cache-line bytes
	add		$r5, $r5, $r3		! $r5 <- upper bound
	andi	$r2, $r2, 0x1f		! $r2 <- bytes left

	! cache-line-mode set loop

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
	smw.bim	$r16, [$r3], $r23, #0 	!$r3 is dst
	bne	$r3, $r5, 4b				!Next cache-line

    .align  2
.Lword_mode:
	srli	$r5, $r2, 2			! $r5 <- n words to set
	beqz	$r5, .Lbyte_mode	! smaller than 4 bytes
	slli	$r5, $r5, 2			! $r5 <- n bytes
	add		$r5, $r5, $r3		! $r5 <- upper bound
	andi	$r2, $r2, 0x3		! $r2 <- bytes left

	! word-mode set loop
	.align	2

2:
	smw.bim	$r1, [$r3], $r1, #0
    bne $r3, $r5, 2b            ! Next word.

    .align  2
.Lbyte_mode:
	beqz	$r2, .Lend
	add	$r5, $r3, $r2			! Set $r5 as upper bound.

	.align	2
3:
	! byte-mode set loops
	sbi.bi	$r1, [$r3], #1
	bne		$r3, $r5, 3b		! Not readch upper bound. Loop.

    .align  2
.Lend:
	ret

END(memset)
libc_hidden_builtin_def (memset)

