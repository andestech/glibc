/* get current floating-point environment.  Andes nds32 version.
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
__fegetenv (fenv_t *envp)
{
  unsigned long int temp;
  unsigned int udfiex_trap;

  _FPU_GETCW (temp);

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    udfiex_trap = __fp_udfiex_crtl (GET_UDF_IEX_TRAP, 0);
    if (udfiex_trap < 0)
      return -1;

    temp = (temp & (~FPU_MASK_SUBOUT)) | udfiex_trap;
  }
  *envp = temp;
  /* Success.  */
  return 0;
}
libm_hidden_def (__fegetenv)
weak_alias (__fegetenv, fegetenv)
libm_hidden_weak (fegetenv)
