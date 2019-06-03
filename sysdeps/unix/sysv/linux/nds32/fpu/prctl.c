/* Operations on a process.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <stdarg.h>
#include <sysdep.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
int prctl (int __option, ...);
libc_hidden_proto(prctl)

int __prctl (int __option, ...)
{
	unsigned long arg1,arg2,arg3,arg4;
	va_list arg;
	va_start (arg, __option);
	arg1 = va_arg (arg, unsigned long);
	arg2 = va_arg (arg, unsigned long);
	arg3 = va_arg (arg, unsigned long);
	arg4 = va_arg (arg, unsigned long);
	va_end (arg);
	return INLINE_SYSCALL(prctl,5,__option,arg1,arg2,arg3,arg4);
}
weak_alias( __prctl,prctl)
libc_hidden_weak (prctl)
