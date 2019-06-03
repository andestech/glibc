/* struct ucontext definition, Andes nds32 version.
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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <bits/types/sigset_t.h>
#include <bits/types/stack_t.h>

/* Context to describe whole processor state.  */

typedef struct {
        unsigned long int __trap_no;
        unsigned long int __error_code;
        unsigned long int __oldmask;
        unsigned long int __nds32_r0;
        unsigned long int __nds32_r1;
        unsigned long int __nds32_r2;
        unsigned long int __nds32_r3;
        unsigned long int __nds32_r4;
        unsigned long int __nds32_r5;
        unsigned long int __nds32_r6;
        unsigned long int __nds32_r7;
        unsigned long int __nds32_r8;
        unsigned long int __nds32_r9;
        unsigned long int __nds32_r10;
        unsigned long int __nds32_r11;
        unsigned long int __nds32_r12;
        unsigned long int __nds32_r13;
        unsigned long int __nds32_r14;
        unsigned long int __nds32_r15;
        unsigned long int __nds32_r16;
        unsigned long int __nds32_r17;
        unsigned long int __nds32_r18;
        unsigned long int __nds32_r19;
        unsigned long int __nds32_r20;
        unsigned long int __nds32_r21;
        unsigned long int __nds32_r22;
        unsigned long int __nds32_r23;
        unsigned long int __nds32_r24;
        unsigned long int __nds32_r25;
        unsigned long int __nds32_fp;
        unsigned long int __nds32_gp;
        unsigned long int __nds32_lp;
        unsigned long int __nds32_sp;
        unsigned long int __nds32_ipc;
        unsigned long int __fault_address;
        unsigned long int __used_math_flag;
        unsigned long long int __fd_regs[32];
        unsigned long int __fpcsr;
        unsigned long int __UDF_trap;
        unsigned long int __zol[3];
} mcontext_t;


/* Userlevel context.  */
typedef struct ucontext_t
  {
    unsigned long int __uc_flags;
    struct ucontext_t *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
  } ucontext_t;
#endif /* _SYS_UCONTEXT_H.  */
