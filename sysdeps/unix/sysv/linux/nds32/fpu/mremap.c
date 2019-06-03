/* Re-map a virtual memory address.
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
#include <sys/mman.h>
#include <sys/syscall.h>

void *__mremap (void *__addr, size_t __old_len, size_t __new_len, int __flags, ...)
{
	unsigned long arg1;
	va_list arg;
	va_start (arg, __flags);
	arg1 = va_arg (arg, int);
	va_end (arg);
	return (void *)INLINE_SYSCALL(mremap,5,__addr,__old_len,__new_len,__flags,arg1);
}
weak_alias (__mremap, mremap)
libc_hidden_def (__mremap)
