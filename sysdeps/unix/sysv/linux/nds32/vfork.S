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


ENTRY (__libc_vfork)
#ifdef PIC
.pic
#endif	

	movi $r0, #0x4111 /* CLONE_VM | CLONE_VFORK | SIGCHLD */
	move $r1, $sp
	movi $r15, __NR_clone
	syscall #0x0
	bltz $r0, 2f
1:
	ret
2:
	sltsi $r1, $r0, -4096
	bnez $r1, 1b;

	j SYSCALL_ERROR_LABEL

PSEUDO_END (__libc_vfork)
#if IS_IN (libc)
weak_alias (__libc_vfork, vfork)
strong_alias (__libc_vfork, __vfork)
libc_hidden_def (__vfork)
#endif
