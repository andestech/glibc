/* vfork for Linux, Andes Linux/nds32 version.
   Copyright (C) 2005-2018 Free Software Foundation, Inc.
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
#define _ERRNO_H    1
#include <bits/errno.h>


ENTRY (__libc_vfork)
#ifdef PIC
.pic
#endif	

        movi $r0, #0x4111 /* 0x4111 = CLONE_VM | CLONE_VFORK | SIGCHLD.  */
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

weak_alias (__libc_vfork, vfork)
strong_alias (__libc_vfork, __vfork)
libc_hidden_def (__vfork)
