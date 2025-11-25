/* Enumerate available IFUNC implementations of a function.  RISCV version.
   Copyright (C) 2024 Free Software Foundation, Inc.
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

#include <ifunc-impl-list.h>
#include <string.h>
#include <sys/hwprobe.h>

#define ARRAY_SIZE(A) (sizeof (A) / sizeof ((A)[0]))

size_t
__libc_ifunc_impl_list (const char *name, struct libc_ifunc_impl *array,
			size_t max)
{
  size_t i = max;
  struct riscv_hwprobe pairs[] = {
    { .key = RISCV_HWPROBE_KEY_IMA_EXT_0 },
    { .key = RISCV_HWPROBE_KEY_CPUPERF_0 },
  };

  bool fast_unaligned = false;
#if HAVE_RISCV_FATTRIBUTE_ZBB
  bool has_zbb = false;
#endif

  if (__riscv_hwprobe (pairs, ARRAY_SIZE (pairs), 0, NULL, 0) == 0)
    {
      struct riscv_hwprobe *pair;

      /* RISCV_HWPROBE_KEY_IMA_EXT_0  */
      pair = &pairs[0];
#if HAVE_RISCV_FATTRIBUTE_ZBB
      if (pair->value & RISCV_HWPROBE_EXT_ZBB)
        has_zbb = true;
#endif

      /* RISCV_HWPROBE_KEY_CPUPERF_0  */
      pair = &pairs[1];
      if ((pair->value & RISCV_HWPROBE_MISALIGNED_MASK)
	   == RISCV_HWPROBE_MISALIGNED_FAST)
        fast_unaligned = true;
    }

  IFUNC_IMPL (i, name, memchr,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, memchr, has_zbb, __memchr_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, memchr, 1, __memchr_generic))

  IFUNC_IMPL (i, name, memcpy,
	      IFUNC_IMPL_ADD (array, i, memcpy, fast_unaligned,
			      __memcpy_noalignment)
	      IFUNC_IMPL_ADD (array, i, memcpy, 1, __memcpy_generic))

  IFUNC_IMPL (i, name, memrchr,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, memrchr, has_zbb, __memrchr_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, memrchr, 1, __memrchr_generic))

  IFUNC_IMPL (i, name, strchrnul,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, strchrnul, has_zbb, __strchrnul_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, strchrnul, 1, __strchrnul_generic))

  IFUNC_IMPL (i, name, strcmp,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, strcmp, has_zbb, __strcmp_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, strcmp, 1, __strcmp_generic))

  IFUNC_IMPL (i, name, strlen,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, strlen, has_zbb, __strlen_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, strlen, 1, __strlen_generic))

  IFUNC_IMPL (i, name, strncmp,
#if HAVE_RISCV_FATTRIBUTE_ZBB
	      IFUNC_IMPL_ADD (array, i, strncmp, has_zbb, __strncmp_zbb)
#endif
	      IFUNC_IMPL_ADD (array, i, strncmp, 1, __strncmp_generic))

  return 0;
}
