/* Control device.
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

#include <sys/ioctl.h>
#include <sys/syscall.h>

int __ioctl (int __fd, unsigned long int __request, ...)
{
	unsigned long arg1;
	va_list arg;
	va_start (arg, __request);
	arg1 = va_arg (arg, unsigned long);
	va_end (arg);
	return INLINE_SYSCALL(ioctl,3,__fd,__request,arg1);
}
libc_hidden_def (__ioctl)
weak_alias (__ioctl, ioctl)
