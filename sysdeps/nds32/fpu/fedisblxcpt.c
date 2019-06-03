/* Disable floating-point exceptions.  Andes nds32 version.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

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
fedisableexcept (int excepts)
{
  unsigned long int new_fpcsr, fpcsr, old_exc;
  unsigned long int act = 0, cmd = GET_UDF_IEX_TRAP;
  int old_udfiex_trap;

  _FPU_GETCW(fpcsr);

  new_fpcsr = fpcsr & (~ENABLE_MASK);

  excepts = (excepts & FE_ALL_EXCEPT) << ENABLE_SHIFT;

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    old_exc = fpcsr & ENABLE_MASK_SUBOUT;
    new_fpcsr |= FPU_MASK_SUBOUT;

    if (excepts & FPU_MASK_SUBOUT) {
      cmd = DISABLE_UDF_IEX_TRAP;
      act = excepts & FPU_MASK_SUBOUT;
    }
    old_udfiex_trap = __fp_udfiex_crtl(cmd, act);
    if (old_udfiex_trap < 0)
      return -1;

    old_exc |= old_udfiex_trap;
  } else
    old_exc = fpcsr & ENABLE_MASK;

  new_fpcsr |= (old_exc & (~excepts));

  _FPU_SETCW (new_fpcsr);


  return old_exc >> ENABLE_SHIFT;
}
