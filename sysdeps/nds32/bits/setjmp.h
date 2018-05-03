/* Define the machine-dependent type `jmp_buf'.  Andes nds32 version.
   Copyright (C) 2003-2018 Free Software Foundation, Inc.
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

#ifndef _BITS_SETJMP_H
#define _BITS_SETJMP_H  1

#if !defined _SETJMP_H && !defined _PTHREAD_H
# error "Never include <bits/setjmp.h> directly; use <setjmp.h> instead."
#endif

#ifndef _ASM

/*
  For nds32, there are 32 general purpose registers.
  1. Caller saved register for parameter passing: $r0 - $r5
  2. Callee saved register: $r6 - $r14
  3. Assembler reserved register: $r15
  4. Caller saved register: $r6 - $r24
  5. Thread pointer register: $r25
  6. Reserved register for kernel space: $r26 - $27
  7. Frame pointer: $r28
  8. Global pointer: $r29
  9. Link register: $r30
  10.Stack pointer: $r31

  Only callee saved register and $r28-$r31 needs to store in jmp_buf.
  Reserved(For 8-byte align if needed)
*/

typedef struct __jmp_buf_internal_tag
  {
    /* Callee-saved registers: $r6 - $r14, $fp, $gp, $lp, $sp: $r28 - $r31.  */
    int __regs[13];
  } __jmp_buf[1] __attribute__((__aligned__ (8)));

#endif

#endif  /* bits/setjmp.h */
