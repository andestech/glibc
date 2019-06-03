/* longjmp, Andes nds32 version.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
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

ENTRY(__longjmp)
	/* Restore registers.  */
	lmw.bim  $r6, [$r0], $r14, #0xf

#ifdef __NDS32_ABI_2FP_PLUS__
	lwi.bi	$r20, [$r0], #4	/* Load $fpcfg.freg to $r20.  */

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz    $r20, .LCFG0         /* Branch if $fpcfg.freg = 0b00.  */
	xori    $r15, $r20, #0b10
	beqz    $r15, .LCFG2         /* Branch if $fpcfg.freg = 0b10.  */
	srli    $r20, $r20, #0b01
	beqz    $r20, .LCFG1         /* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
	fldi.bi	$fd31, [$r0], #8
	fldi.bi	$fd30, [$r0], #8
	fldi.bi	$fd29, [$r0], #8
	fldi.bi	$fd28, [$r0], #8
	fldi.bi	$fd27, [$r0], #8
	fldi.bi	$fd26, [$r0], #8
	fldi.bi	$fd25, [$r0], #8
	fldi.bi	$fd24, [$r0], #8
.LCFG2:
	fldi.bi	$fd10, [$r0], #8
	fldi.bi	$fd9, [$r0], #8
	fldi.bi	$fd8, [$r0], #8
.LCFG1:
	fldi.bi	$fd7, [$r0], #8
	fldi.bi	$fd6, [$r0], #8
	fldi.bi	$fd5, [$r0], #8
	fldi.bi	$fd4, [$r0], #8
.LCFG0:
	fldi.bi	$fd3, [$r0], #8
#endif /* __NDS32_ABI_2FP_PLUS__.  */

	/* Return error code. Make sure error code is not 0.  */
	bnez  $r1, .Ldone
	movi  $r1, #1
.Ldone:
	addi  $r0, $r1, #0
	ret
END(__longjmp)
