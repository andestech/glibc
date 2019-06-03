/* Private floating point rounding and exceptions handling.
   Andes NDS32 version.
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

#ifndef NDS32_MATH_PRIVATE_H
#define NDS32_MATH_PRIVATE_H 1

#include <fenv.h>
#include <fpu_control.h>

extern long int __fp_udfiex_crtl(int cmd, int act);

static __always_inline int
libc_feholdexcept_nds32 (fenv_t *envp)
{
  int fpcsr, old_udfiex_trap;

  _FPU_GETCW(fpcsr);

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    /* Store the environment.  */
    *envp = (fpcsr & (~FPU_MASK_SUBOUT));

    /* Get underflow trap status and disable underflow trap.  */
    old_udfiex_trap = __fp_udfiex_crtl (DISABLE_UDF_IEX_TRAP, FPU_MASK_SUBOUT);
    if (old_udfiex_trap >= 0)
       *envp |= old_udfiex_trap;

    /* Now set left exceptions to non-stop and clear all exception flags.  */
    fpcsr &= ~(ENABLE_MASK_SUBOUT | FE_ALL_EXCEPT);
  } else {
    *envp = fpcsr;
    fpcsr &= ~(ENABLE_MASK | FE_ALL_EXCEPT);
  }
  _FPU_SETCW (fpcsr);
  return 0;
}

#define libc_feholdexcept  libc_feholdexcept_nds32
#define libc_feholdexceptf libc_feholdexcept_nds32
#define libc_feholdexceptl libc_feholdexcept_nds32

static __always_inline void
libc_fesetround_nds32 (int round)
{
  int temp;

  _FPU_GETCW (temp);
  temp = (temp & ~ROUND_MOD_MASK) | round;
  _FPU_SETCW (temp);
}

#define libc_fesetround  libc_fesetround_nds32
#define libc_fesetroundf libc_fesetround_nds32
#define libc_fesetroundl libc_fesetround_nds32

static __always_inline void
libc_feholdexcept_setround_nds32 (fenv_t *envp, int round)
{
  libc_feholdexcept_nds32 (envp);
  libc_fesetround_nds32 (round);
}

#define libc_feholdexcept_setround  libc_feholdexcept_setround_nds32
#define libc_feholdexcept_setroundf libc_feholdexcept_setround_nds32
#define libc_feholdexcept_setroundl libc_feholdexcept_setround_nds32

static __always_inline int
libc_fetestexcept_nds32 (int excepts)
{
  fexcept_t temp;

  /* Get current exceptions.  */
  _FPU_GETCW(temp);

  return temp & excepts & FE_ALL_EXCEPT;
}

#define libc_fetestexcept  libc_fetestexcept_nds32
#define libc_fetestexceptf libc_fetestexcept_nds32
#define libc_fetestexceptl libc_fetestexcept_nds32

static __always_inline int
libc_fesetenv_nds32 (const fenv_t *envp)
{
  fpu_control_t fpcsr, fpcsr_new;
  unsigned long int cmd = DISABLE_UDF_IEX_TRAP, act = FPU_MASK_SUBOUT;

  _FPU_GETCW (fpcsr);

  fpcsr_new = (fpcsr & (_FPU_ENV_RESERVED));

  if (envp == FE_DFL_ENV)
    fpcsr_new |= _FPU_DEFAULT;
  else if (envp == FE_NOMASK_ENV) {
    fpcsr_new |= _FPU_IEEE;
    cmd = ENABLE_UDF_IEX_TRAP;
  } else {
    fpcsr_new |= (*envp & ~(_FPU_ENV_RESERVED));
    /* At first, disable UDF and IEX trap.  */
    if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
      if (__fp_udfiex_crtl (cmd, act) < 0)
        return -1;
    }
    /* Then, follow requirements to enable UDF and IEX trap.  */
    cmd = ENABLE_UDF_IEX_TRAP;
    act = *envp & FPU_MASK_SUBOUT;
  }

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    fpcsr_new |= FPU_MASK_SUBOUT;
    if (__fp_udfiex_crtl (cmd, act) < 0)
      return -1;
  }
  _FPU_SETCW (fpcsr_new);
  return 0;
}

#define libc_fesetenv  libc_fesetenv_nds32
#define libc_fesetenvf libc_fesetenv_nds32
#define libc_fesetenvl libc_fesetenv_nds32
#define libc_feresetround_noex  libc_fesetenv_nds32
#define libc_feresetround_noexf libc_fesetenv_nds32
#define libc_feresetround_noexl libc_fesetenv_nds32

static __always_inline int
libc_feupdateenv_test_nds32 (const fenv_t *envp, int ex)
{
  fpu_control_t fpcsr, fpcsr_new, old_exc;
  unsigned long int cmd = DISABLE_UDF_IEX_TRAP, act = FPU_MASK_SUBOUT;
  unsigned long int raise_exc;

  _FPU_GETCW (fpcsr);
  old_exc = fpcsr & FE_ALL_EXCEPT;
  fpcsr_new = fpcsr & (_FPU_ENV_RESERVED | FE_ALL_EXCEPT);


  if (envp == FE_DFL_ENV)
    fpcsr_new |= _FPU_DEFAULT;
  else if (envp == FE_NOMASK_ENV) {
    fpcsr_new |= _FPU_IEEE;
    cmd = ENABLE_UDF_IEX_TRAP;
  } else {
    fpcsr_new |= (*envp & ~_FPU_ENV_RESERVED);
    /* At first, disable UDF and IEX trap.  */
    if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
      if (__fp_udfiex_crtl (cmd, act) < 0)
        return -1;
    }
    /* Then, follow requirements to enable UDF and IEX trap.  */
    cmd = ENABLE_UDF_IEX_TRAP;
    act = *envp & FPU_MASK_SUBOUT;
  }

  raise_exc = old_exc & (fpcsr_new >> ENABLE_SHIFT);

  if (__KERNEL_SUPPORT_SUBNOR_OUTPUT) {
    fpcsr_new |= FPU_MASK_SUBOUT;
    if (__fp_udfiex_crtl (cmd, act) < 0)
      return -1;
  }

  _FPU_SETCW (fpcsr_new);
  if (raise_exc)
    __feraiseexcept (raise_exc);
  return old_exc & ex;
}
#define libc_feupdateenv_test  libc_feupdateenv_test_nds32
#define libc_feupdateenv_testf libc_feupdateenv_test_nds32
#define libc_feupdateenv_testl libc_feupdateenv_test_nds32

static __always_inline int
libc_feupdateenv_nds32 (const fenv_t *envp)
{
  return libc_feupdateenv_test_nds32(envp, 0);
}

#define libc_feupdateenv  libc_feupdateenv_nds32
#define libc_feupdateenvf libc_feupdateenv_nds32
#define libc_feupdateenvl libc_feupdateenv_nds32

static __always_inline void
libc_feholdsetround_nds32 (fenv_t *envp, int round)
{
  fpu_control_t fpcsr;
  
  _FPU_GETCW (fpcsr);
  *envp = fpcsr & ROUND_MOD_MASK;
  fpcsr = (fpcsr & ~ROUND_MOD_MASK) | (round & ROUND_MOD_MASK);
  _FPU_SETCW (fpcsr);
}

#define libc_feholdsetround  libc_feholdsetround_nds32
#define libc_feholdsetroundf libc_feholdsetround_nds32
#define libc_feholdsetroundl libc_feholdsetround_nds32

static __always_inline void
libc_feresetround_nds32 (fenv_t *envp)
{
  int temp;

  _FPU_GETCW (temp);
  temp = (temp & ~ROUND_MOD_MASK) | (*envp & ROUND_MOD_MASK);
  _FPU_SETCW (temp);
}

#define libc_feresetround  libc_feresetround_nds32
#define libc_feresetroundf libc_feresetround_nds32
#define libc_feresetroundl libc_feresetround_nds32


#include_next <fenv_private.h>

#endif
