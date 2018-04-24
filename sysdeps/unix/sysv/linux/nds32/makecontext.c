/* Create new context, Andes nds32 version.
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

#include <stdarg.h>
#include <stdint.h>
#include <sys/ucontext.h>

void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  extern void __startcontext (void);
  unsigned long int *sp;
  unsigned long int *regptr;
  va_list ap;
  int i;

  sp = (unsigned long int *)
    ((uintptr_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size);

  /* Allocate stack for 7-th, 8-th, ..., n-th arguments.  */
  sp -= argc <= 6 ? 0 : argc - 6;

  /* Keep the stack aligned.  */
  sp = (unsigned long int *) (((uintptr_t) sp) & -8L);

  ucp->uc_mcontext.nds32_r6 = (uintptr_t) ucp->uc_link;
  ucp->uc_mcontext.nds32_sp = (uintptr_t) sp;
  ucp->uc_mcontext.nds32_ipc = (uintptr_t) func;
  ucp->uc_mcontext.nds32_lp = (uintptr_t) &__startcontext;

  va_start (ap, argc);
  regptr = &ucp->uc_mcontext.nds32_r0;
  for (i = 0; i < argc; ++i)
    if (i < 6)
      *regptr++ = va_arg (ap, unsigned long int);
    else
      sp[i - 6] = va_arg (ap, unsigned long int);

  va_end (ap);

}
weak_alias (__makecontext, makecontext)
