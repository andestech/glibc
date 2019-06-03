/* Store current floating-point control modes.  Andes nds32 version.
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

#include <fenv.h>
#include <fpu_control.h>
#include <fenv_private.h>

int
fegetmode (femode_t *modep)
{
  _FPU_GETCW (*modep);

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    int old_udfiex_trap;

    old_udfiex_trap = __fp_udfiex_crtl (GET_UDF_IEX_TRAP, 0);
    if (old_udfiex_trap < 0)
      return -1;

    *modep = (*modep & (~FPU_MASK_SUBOUT)) | old_udfiex_trap;
  }

  return 0;
}
