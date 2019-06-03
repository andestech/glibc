/* Set the FPU control word.  Andes nds32 version
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

#include <math.h>
#include <ldsodefs.h>
#include <fpu_control.h>
#include <libc-internal.h>

void
__setfpucw (fpu_control_t set)
{

  fpu_control_t cw;
  /* Fetch the current control word.  */
  _FPU_GETCW (cw);

  /* Preserve the reserved bits, and set the rest as the user
     specified (or the default, if the user gave zero).  */
  cw &= _FPU_RESERVED;
  cw |= set & ~_FPU_RESERVED;


  /* GLRO(dl_fpu_control) == _FPU_DEFAULT_SUPPORT_SUBNORMAL_OUTPUT indicates
   * Kernel will recalculate the underflow operation to support denormalize
   * output. To ensure each that an underflow exception is issued each time a
   * denormalize output occurs, the enable bit of underflow exception in
   * $fpcsr shall be set by default.
   */
  if (GLRO(dl_fpu_control) == _FPU_DEFAULT_SUPPORT_SUBNORMAL_OUTPUT) {
    __fpu_control = GLRO(dl_fpu_control);
    cw |= FPU_MASK_SUBOUT;
  }
  _FPU_SETCW (cw);

}
