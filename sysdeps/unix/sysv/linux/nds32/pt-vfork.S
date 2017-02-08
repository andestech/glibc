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

/* Clone the calling process, but without copying the whole address space.
   The calling process is suspended until the new process exits or is
   replaced by a call to `execve'.  Return -1 for errors, 0 to the new process,
   and the process ID of the new process to the old process.  */


ENTRY (__vfork)
#ifdef PIC
.pic
#endif	

	movi $r0, #0x4111 /* CLONE_VM | CLONE_VFORK | SIGCHLD */
	move $r0, $sp
	movi $r15, __NR_clone
	syscall 0x0
	bltz $r0, 2f
1:
	ret
2:
	sltsi $r1, $r0, -4096
	bnez $r1, 1b;

#ifdef PIC
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

	sethi	$r15,	hi20(C_SYMBOL_NAME(__syscall_error)@PLT)
	ori     $r15,	$r15, lo12(C_SYMBOL_NAME(__syscall_error)@PLT)
	add     $r15,	$r15, 	$gp

	! jump to SYSCALL_ERROR
	jral		$r15
	popm	$gp,	$lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore lp
	.cfi_restore gp
#else
        j C_SYMBOL_NAME(__syscall_error)
#endif
    ret

PSEUDO_END (__vfork)
libc_hidden_def (__vfork)

weak_alias (__vfork, vfork)
strong_alias (__vfork, __libc_vfork)
