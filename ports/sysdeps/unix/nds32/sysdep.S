/* Copyright (C) 1991,92,93,94,95,96,97,98,2002,03 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sysdep.h>
#define _ERRNO_H
#include <bits/errno.h>

#undef USE_EMUTLS

.text

#ifdef USE_EMUTLS
.globl C_SYMBOL_NAME(__emutls_v.errno)
#else
// .globl C_SYMBOL_NAME(errno)
#ifndef NOT_IN_libc
# define SYSCALL_ERROR_ERRNO __libc_errno
#else
# define SYSCALL_ERROR_ERRNO errno
#endif
#endif
.globl syscall_error

#undef syscall_error
#ifdef NO_UNDERSCORES
__syscall_error:
#else
syscall_error:
#endif

#ifdef PIC
	/* set GP register */
	/* DOTO: If syscall_error is called by libc's function, $gp has set before
	   entering syscall_error() function. Therefore, the following procedure
	   for setting $gp is duplicated. Find a better method to skip following
	   procedure if the caller function is blonged to libc. */
	pushm	$gp, $lp
	.cfi_adjust_cfa_offset 8
	.cfi_rel_offset gp, 0
	.cfi_rel_offset lp, 4
# ifdef __NDS32_N1213_43U1H__
	jal	2f
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_)
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_ + 4)
	add	$gp,	$gp,	$lp
# else
#  ifdef __NDS32_BASELINE_V3__
        sethi   $gp, HI20(_GLOBAL_OFFSET_TABLE_ -  8)
        ori     $gp, $gp, LO12(_GLOBAL_OFFSET_TABLE_ - 4)
        add5.pc  $gp
#  else
        sethi   $gp, hi20(_GLOBAL_OFFSET_TABLE_ - 8)
        ori     $gp, $gp, lo12(_GLOBAL_OFFSET_TABLE_ - 4)
        mfusr   $ta, $pc
        add     $gp, $ta, $gp
#  endif
# endif
#endif	

#if defined (EWOULDBLOCK_sys) && EWOULDBLOCK_sys != EAGAIN
	/* We translate the system's EWOULDBLOCK error into EAGAIN.
	   The GNU C library always defines EWOULDBLOCK==EAGAIN.
	   EWOULDBLOCK_sys is the original number.  */
	   push	$t0
	   .cfi_adjust_cfa_offset 4
	   li	$t0, EWOULDBLOCK_sys
	   bne	$r0, $t0, 1f
	   pop	$t0
	   .cfi_adjust_cfa_offset -4
	   li	$r0, EAGAIN
1:
#endif

#ifdef _LIBC_REENTRANT
	push	$lp
	.cfi_adjust_cfa_offset 4
	.cfi_rel_offset lp, 0
# ifdef OLD_ABI
	push	$r5
	.cfi_adjust_cfa_offset 4
# else
	push	$r0
	.cfi_adjust_cfa_offset 4
# endif

# if defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__)
# else
	addi	$sp,	$sp, -24
	.cfi_adjust_cfa_offset 24
# endif

# ifdef PIC
	bal	C_SYMBOL_NAME(__errno_location@PLT)
# else
	bal	C_SYMBOL_NAME(__errno_location)
# endif
# if defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__)
# else
	addi	$sp,	$sp, 24
	.cfi_adjust_cfa_offset -24
# endif
	
	pop	$r1
	.cfi_adjust_cfa_offset -4
	
# ifdef OLD_ABI
	swi	$r1, [$r5]
	li	$r5, -1
# else
	swi	$r1, [$r0]
	li	$r0, -1
# endif
	pop	$lp
	.cfi_adjust_cfa_offset -4
	.cfi_restore lp
# ifdef PIC
	/* restore GP register */
	popm	$gp, $lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore gp
	.cfi_restore lp
# endif
2:
	ret
#else
# ifndef	PIC
	l.w	$r1, L1
#  ifdef OLD_ABI
	swi	$r5, [$r1]
	li	$r5, -1
#  else
	swi	$r0, [$r1]
	li	$r0, -1
#  endif
	ret

#  ifdef USE_EMUTLS
L1:     .long C_SYMBOL_NAME(__emutls_v.errno)
#  else
// L1:	.long C_SYMBOL_NAME(errno)
#  endif

# else
#  ifdef OLD_ABI
	s.w	$r5, errno@GOTOFF
	li	$r5, -1
#  else
#   ifdef USE_EMUTLS
	s.w	$r0, __emutls_v.errno@GOTOFF
#   else
//	s.w	$r0, errno@GOTOFF
	s.w	$r0, SYSCALL_ERROR_ERRNO@GOTOFF
#   endif
	li	$r0, -1
#  endif
	
	/* restore GP register */
	popm	$gp, $lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore gp
	.cfi_restore lp
2:
	ret

# endif
#endif

#undef __syscall_error
END (__syscall_error)

