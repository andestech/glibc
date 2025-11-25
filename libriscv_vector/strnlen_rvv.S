/* RVV versions strnlen.  RISC-V version.
   Copyright (C) 2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by: Nick Knight <nick.knight@sifive.com>

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

#include <sysdep.h>
#include <sys/asm.h>

#define pStr a0
#define pCopyStr a2
#define iRetValue a0
#define iMaxlen a1
#define iCurrentVL a3
#define iEndOffset a4

#define ELEM_LMUL_SETTING m1
#define vStr v0
#define vMaskEnd v8


ENTRY(__strnlen_rvv)
.option push
.option arch,+v

    mv pCopyStr, pStr
    mv iRetValue, iMaxlen
L(strnlen_loop):
    beqz iMaxlen, L(end_strnlen_loop)
    vsetvli zero, iMaxlen, e8, ELEM_LMUL_SETTING, ta, ma
    vle8ff.v vStr, (pCopyStr)
    vmseq.vi vMaskEnd, vStr, 0
    vfirst.m iEndOffset, vMaskEnd /* first occurence of \0 */
    csrr iCurrentVL, vl
    add pCopyStr, pCopyStr, iCurrentVL
    sub iMaxlen, iMaxlen, iCurrentVL
    bltz iEndOffset, L(strnlen_loop)
    add iMaxlen, iMaxlen, iCurrentVL
    sub iRetValue, iRetValue, iMaxlen
    add iRetValue, iRetValue, iEndOffset
L(end_strnlen_loop):
    ret

.option pop
END(__strnlen_rvv)
libc_hidden_builtin_def (__strnlen_rvv)
