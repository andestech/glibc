/* Copyright (C) 1999, 2002, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Philip Blundell <philb@gnu.org>.

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
/*
   Revision History:

    Sep.07.2006     Created by Harry.
                    Initial porting refered from ARM platform.
*/

#include <sysdep.h>
#define _ERRNO_H    1
#include <bits/errno.h>
#include <kernel-features.h>

/* Clone the calling process, but without copying the whole address space.
   The calling process is suspended until the new process exits or is
   replaced by a call to `execve'.  Return -1 for errors, 0 to the new process,
   and the process ID of the new process to the old process.  */


ENTRY (__vfork)
#ifdef PIC
.pic
#endif	

#ifdef __NR_vfork
# ifdef SAVE_PID
    SAVE_PID
# endif
    syscall __NR_vfork
# ifdef RESTORE_PID
    RESTORE_PID
# endif
    bltz $r0, 2f
1:
    ret
2:
    sltsi $r1, $r0, -4096
    bnez $r1, 1b;

# ifdef __ASSUME_VFORK_SYSCALL
#  ifdef PIC
	! set r1 as gp
	pushm	$gp,	$lp
	.cfi_adjust_cfa_offset 8
	.cfi_rel_offset gp, 0
	.cfi_rel_offset lp, 4
	mfusr 	$r15, 	$PC
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_+4)
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_+8)
	add	$gp,	$gp,	$r15

	! r15=C_SYMBOL_NAME(__syscall_error)@PLT
	sethi	$r15, 	hi20(C_SYMBOL_NAME(__syscall_error)@PLT)
	ori	$r15,	$r15, lo12(C_SYMBOL_NAME(__syscall_error)@PLT)
	add	$r15, 	$r15, 	$gp
	
	! jump to SYSCALL_ERROR
	jral		$r15
	popm	$gp,	$lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore lp
	.cfi_restore gp
	ret
#  else
	j C_SYMBOL_NAME(__syscall_error)
#  endif
# else
    /* Check if vfork syscall is known at all. */
	li $r1, -ENOSYS
	beq	$r0, $r1, 1f

# ifdef PIC
3:
        ! set r1 as gp
	pushm	$gp,	$lp
	.cfi_adjust_cfa_offset 8
	.cfi_rel_offset gp, 0
	.cfi_rel_offset lp, 4
	mfusr	$r15,   $PC
	sethi   $gp,    hi20(_GLOBAL_OFFSET_TABLE_+4)
	ori     $gp,    $gp,    lo12(_GLOBAL_OFFSET_TABLE_+8)
	add     $gp,    $gp,    $r15

	! r15=C_SYMBOL_NAME(__syscall_error)@PLT
	sethi	$r15,	hi20(C_SYMBOL_NAME(__syscall_error)@PLT)
	ori     $r15,	$r15, lo12(C_SYMBOL_NAME(__syscall_error)@PLT)
	add     $r15,	$r15, 	$gp

	! jump to SYSCALL_ERROR
	jral		$r15
	popm	$gp,	$lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore lp
	.cfi_restore gp
	ret
# else
        j C_SYMBOL_NAME(__syscall_error)
# endif
1: 
# endif
#endif

#ifndef __ASSUME_VFORK_SYSCALL
    /* If we don't have vfork, fork is close enough. */
#ifdef OLD_ABI	 
	li $r7, __NR_fork
	syscall LIB_SYSCALL
	bgez $r5, 1f
	sltsi $r0, $r5, -4096
	bnez	$r0, 1f
#else
	syscall __NR_fork
	bgez $r0, 1f
	sltsi $r1, $r0, -4096
	bnez	$r1, 1f
#endif

# ifdef PIC
        b       3b
# else
        j C_SYMBOL_NAME(__syscall_error)
# endif
1:
    ret

#elif !defined __NR_vfork
# error "__NR_vfork not available and __ASSUME_VFORK_SYSCALL defined"
#endif

PSEUDO_END (__vfork)

libc_hidden_def (__vfork)

weak_alias (__vfork, vfork)
