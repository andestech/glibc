/* Multiple versions of memrchr.
   All versions must be listed in ifunc-impl-list.c.
   Copyright (C) 2017-2024 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */

#if IS_IN (libc)
/* Redefine memrchr so that the compiler won't complain about the type
   mismatch with the IFUNC selector in strong_alias, below.  */
# undef memrchr
# undef __memrchr
# define memrchr __redirect_memrchr
# define __memrchr __redirect___memrchr
# include <stdint.h>
# include <string.h>
# include <ifunc-init.h>
# include <riscv-ifunc.h>
# include <sys/hwprobe.h>

extern __typeof (__redirect_memrchr) __libc_memrchr;

extern __typeof (__redirect_memrchr) __memrchr_generic attribute_hidden;
extern __typeof (__redirect_memrchr) __memrchr_zbb attribute_hidden;

static inline __typeof (__redirect_memrchr) *
select_memrchr_ifunc (uint64_t dl_hwcap, __riscv_hwprobe_t hwprobe_func)
{
#if HAVE_RISCV_FATTRIBUTE_ZBB
  unsigned long long int v;
  if (__riscv_hwprobe_one (hwprobe_func, RISCV_HWPROBE_KEY_IMA_EXT_0, &v) == 0
      && (v & RISCV_HWPROBE_EXT_ZBB))
    return __memrchr_zbb;
#endif

  return __memrchr_generic;
}

riscv_libc_ifunc (__libc_memrchr, select_memrchr_ifunc);

# undef memrchr
# undef __memrchr
strong_alias (__libc_memrchr, __memrchr);
weak_alias (__memrchr, memrchr)
# ifdef SHARED
__hidden_ver1 (memrchr, __GI___memrchr, __redirect_memrchr)
  __attribute__ ((visibility ("hidden"))) __attribute_copy__ (memrchr);
# endif
#else
# include <string/memrchr.c>
#endif
