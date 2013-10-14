/* Copyright (C) 2003, 2004 Free Software Foundation, Inc.
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

#include <errno.h>
#include <fcntl.h>
#include <sysdep.h>
#include <kernel-features.h>

int __posix_fadvise64_l64 (int fd, off64_t offset, off64_t len, int advise);
int __posix_fadvise64_l32 (int fd, off64_t offset, size_t len, int advise);

/* Advice the system about the expected behaviour of the application with
   respect to the file associated with FD.  */

int
__posix_fadvise64_l64 (int fd, off64_t offset, off64_t len, int advise)
{
#ifdef __NR_fadvise64_64
  INTERNAL_SYSCALL_DECL (err);
  int ret = INTERNAL_SYSCALL (fadvise64_64, err, 6, fd, advise,
			      __LONG_LONG_PAIR ((long)(offset >> 32), (long)offset),
			      __LONG_LONG_PAIR ((long)(len >> 32), (long)len));
  if (ret < 0) {
    return -ret;
  } else
    return 0;
#else
  return ENOSYS;
#endif
}

#include <shlib-compat.h>

#if SHLIB_COMPAT(libc, GLIBC_2_2, GLIBC_2_3_3)

int
attribute_compat_text_section
__posix_fadvise64_l32 (int fd, off64_t offset, size_t len, int advise)
{
  return __posix_fadvise64_l64 (fd, offset, len, advise);
}

versioned_symbol (libc, __posix_fadvise64_l64, posix_fadvise64, GLIBC_2_3_3);
compat_symbol (libc, __posix_fadvise64_l32, posix_fadvise64, GLIBC_2_2);
#else
strong_alias (__posix_fadvise64_l64, posix_fadvise64);
#endif
