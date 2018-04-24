/* System call interface, Andes nds32 version.
   Copyright (C) 2001-2018 Free Software Foundation, Inc.
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

#include <errno.h>
#include <stdarg.h>
#include <sysdep.h>
long int syscall (long int __sysno, ...)
{
  int result;
  unsigned long arg1, arg2, arg3, arg4, arg5, arg6;
  va_list arg;
  va_start (arg, __sysno);
  arg1 = va_arg (arg, unsigned long);
  arg2 = va_arg (arg, unsigned long);
  arg3 = va_arg (arg, unsigned long);
  arg4 = va_arg (arg, unsigned long);
  arg5 = va_arg (arg, unsigned long);
  arg6 = va_arg (arg, unsigned long);
  va_end (arg);
  result =
      INLINE_SYSCALL_NCS (__sysno, 6, arg1, arg2, arg3, arg4, arg5, arg6);
  return result;
}
