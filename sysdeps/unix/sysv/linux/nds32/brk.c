/* brk system call for Linux/NDS32.
   Copyright (C) 1995, 1996 Free Software Foundation, Inc.
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
// ============================================================================
//  Copyright (C) 2006-2007 Andes Technology, Inc.
//  This file is part of GLIBC and is licensed under the GPL.
//  See the file COPYING for conditions for redistribution.
//
//  Abstract:
//
//    This program is target-dependent code for Andes NDS32 architecture.
//
//  Revision History:
//
//    Sep.07.2006     Created by Harry.
//                    Initial porting refered from ARM platform.
//
//  Note:
//
//    TBD means to-be-done, to-be-develope, whatever.
//    WIP means working-in-progress.
//
// ============================================================================
#include <errno.h>
#include <unistd.h>
#include <sysdep.h>

/* This must be initialized data because commons can't have aliases.  */
void *__curbrk = 0;

int
__brk (void *addr)
{
  void *newbrk;

  __asm__ __volatile__ ("move       $r0, %1      \n\t"   // save the argment in r0
			"move	    $r15,%2	 \n\t"
			"syscall    0x0       \n\t"   // do the system call
                        "move       %0, $r0      \n\t"   // keep the return value
                        : "=r"(newbrk)
                        : "r"(addr), "i"(SYS_ify(brk))
                        : "$r0","$r15");

  __curbrk = newbrk;

  if (newbrk < addr)
    {
      __set_errno (ENOMEM);
      return -1;
    }

  return 0;
}
weak_alias (__brk, brk)

