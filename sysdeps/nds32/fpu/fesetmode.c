/* Install given floating-point control modes.  Andes nds32 version.
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
fesetmode (const femode_t *modep)
{
  fpu_control_t fpcsr, new_fpcsr;
  int cmd = DISABLE_UDF_IEX_TRAP, act = FPU_MASK_SUBOUT;

  _FPU_GETCW (fpcsr);
  new_fpcsr = fpcsr & _FPU_MOD_RESERVED;

  if (modep == FE_DFL_MODE)
    new_fpcsr |= _FPU_DEFAULT;
  else {
    new_fpcsr |= (*modep & ~_FPU_MOD_RESERVED);
    /* At first, disable UDF and IEX trap.  */
    if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
      if (__fp_udfiex_crtl (cmd, act) < 0)
        return -1;
    }
    cmd = ENABLE_UDF_IEX_TRAP;
    act = *modep & FPU_MASK_SUBOUT;
  }

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    new_fpcsr |= FPU_MASK_SUBOUT;
    if (__fp_udfiex_crtl (cmd, act) < 0)
      return -1;
  }

  _FPU_SETCW (new_fpcsr);
  return 0;
}
