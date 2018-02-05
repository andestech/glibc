/* Copyright (C) 1998, 1999, 2001 Free Software Foundation, Inc.
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

/* System V/ARM ABI compliant context switching support.  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <bits/types/sigset_t.h>
#include <bits/types/stack_t.h>

/* We need the signal context definitions even if they are not used
 *    included in <signal.h>.  */

#ifdef __USE_MISC
# define __ctx(fld) fld
#else
# define __ctx(fld) __ ## fld
#endif

#ifdef __USE_MISC
# include <sys/procfs.h>


typedef elf_greg_t greg_t;

/* Container for all general registers.  */
typedef elf_gregset_t gregset_t;

/* Structure to describe FPU registers.  */
typedef elf_fpregset_t	fpregset_t;
#endif

/* Context to describe whole processor state.  This only describes
   the core registers; coprocessor registers get saved elsewhere
   (e.g. in uc_regspace, or somewhere unspecified on the stack
   during non-RT signal handlers).  */


typedef struct {
        unsigned long __ctx(trap_no);
        unsigned long __ctx(error_code);
        unsigned long __ctx(oldmask);
        unsigned long __ctx(nds32_r0);
        unsigned long __ctx(nds32_r1);
        unsigned long __ctx(nds32_r2);
        unsigned long __ctx(nds32_r3);
        unsigned long __ctx(nds32_r4);
        unsigned long __ctx(nds32_r5);
        unsigned long __ctx(nds32_r6);
        unsigned long __ctx(nds32_r7);
        unsigned long __ctx(nds32_r8);
        unsigned long __ctx(nds32_r9);
        unsigned long __ctx(nds32_r10);
        unsigned long __ctx(nds32_r11);
        unsigned long __ctx(nds32_r12);
        unsigned long __ctx(nds32_r13);
        unsigned long __ctx(nds32_r14);
        unsigned long __ctx(nds32_r15);
        unsigned long __ctx(nds32_r16);
        unsigned long __ctx(nds32_r17);
        unsigned long __ctx(nds32_r18);
        unsigned long __ctx(nds32_r19);
        unsigned long __ctx(nds32_r20);
        unsigned long __ctx(nds32_r21);
        unsigned long __ctx(nds32_r22);
        unsigned long __ctx(nds32_r23);
        unsigned long __ctx(nds32_r24);
        unsigned long __ctx(nds32_r25);
        unsigned long __ctx(nds32_fp); /* $r28 */
        unsigned long __ctx(nds32_gp); /* $r29 */
        unsigned long __ctx(nds32_lp); /* $r30 */
        unsigned long __ctx(nds32_sp); /* $r31 */
        unsigned long __ctx(nds32_ipc);
        unsigned long __ctx(fault_address);
        unsigned long __ctx(used_math_flag);
        /* FPU Registers */
        unsigned long __ctx(zol)[3];
} mcontext_t;


/* Userlevel context.  */
typedef struct ucontext_t
  {
    unsigned long int __ctx(uc_flags);
    struct ucontext_t *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
  } ucontext_t;
#endif /* sys/ucontext.h */
