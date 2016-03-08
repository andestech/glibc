/* Copyright (C) 2003, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Define the machine-dependent type `jmp_buf'.  NDS32 version. */
#ifndef _BITS_SETJMP_H
#define _BITS_SETJMP_H  1

#ifndef _SETJMP_H
// # error "Never include <bits/setjmp.h> directly; use <setjmp.h> instead." // FIXME, dirty fix.
#endif

#ifndef _ASM
typedef struct
  {
    /* Callee-saved registers: r6 - r14,
     * fp, gp, lp, sp: r28 - r31.  */
    int __regs[13];

    /* Floating-Point Configuration Register.  */
    int __fpcfg;

    /* Callee-saved fp registers pointer.  */
    int __fpregs[32];

  } __jmp_buf[1] __attribute__((__aligned__ (8)));
#endif

/*
r6 r7 r8 r9 r10 r11 r12 r13 r14 r28 r29 r30 r31
 0  1  2  3   4   5   6   7   8   9  10  11  12
*/

#define __JMP_BUF_SP            12

/* Test if longjmp to JMPBUF would unwind the frame
   containing a local variable at ADDRESS.  */
/* #define _JMPBUF_UNWINDS(jmpbuf, address) */

#define _JMPBUF_UNWINDS(jmpbuf, address, demangle) \
  ((void *) (address) < &(jmpbuf)[0].__regs[__JMP_BUF_SP])

#endif  /* bits/setjmp.h */
