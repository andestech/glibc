/* RVV versions strlen.  RISC-V version.
   Copyright (C) 2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jerry Shih <jerry.shih@sifive.com>.

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

#define iResult a0
#define pStr a0
#define pCopyStr a1
#define iVL a2
#define iCurrentVL a2
#define iEndOffset a3

#define ELEM_LMUL_SETTING m2
#define vStr v0
#define vMaskEnd v2

ENTRY(__strlen_rvv)
.option push
.option arch,+v

    mv pCopyStr, pStr
L(loop):
    vsetvli iVL, zero, e8, ELEM_LMUL_SETTING, ta, ma
    vle8ff.v vStr, (pCopyStr)
    csrr iCurrentVL, vl
    vmseq.vi vMaskEnd, vStr, 0
    vfirst.m iEndOffset, vMaskEnd
    add pCopyStr, pCopyStr, iCurrentVL
    bltz iEndOffset, L(loop)

    add pStr, pStr, iCurrentVL
    add pCopyStr, pCopyStr, iEndOffset
    sub iResult, pCopyStr, iResult

    ret

.option pop
END(__strlen_rvv)

libc_hidden_builtin_def (__strlen_rvv)
