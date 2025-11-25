/* RVV versions strcat.  RISC-V version.
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

#define pDst a0
#define pSrc a1
#define pDstPtr a2

#define iVL a3
#define iCurrentVL a4
#define iActiveElemPos a5

#define ELEM_LMUL_SETTING m1
#define vMask1 v0
#define vMask2 v1
#define vStr1 v8
#define vStr2 v16

ENTRY(__strcat_rvv)
.option push
.option arch,+v

    mv pDstPtr, pDst

    /* Perform `strlen(dst)`.  */
L(strlen_loop):
    vsetvli iVL, zero, e8, ELEM_LMUL_SETTING, ta, ma

    vle8ff.v vStr1, (pDstPtr)
    vmseq.vx vMask1, vStr1, zero
    csrr iCurrentVL, vl
    vfirst.m iActiveElemPos, vMask1
    add pDstPtr, pDstPtr, iCurrentVL
    bltz iActiveElemPos, L(strlen_loop)

    sub pDstPtr, pDstPtr, iCurrentVL
    add pDstPtr, pDstPtr, iActiveElemPos

    /* Perform `strcpy(dst, src)`.  */
L(strcpy_loop):
    vsetvli iVL, zero, e8, ELEM_LMUL_SETTING, ta, ma

    vle8ff.v vStr1, (pSrc)
    vmseq.vx vMask2, vStr1, zero
    csrr iCurrentVL, vl
    vfirst.m iActiveElemPos, vMask2
    vmsif.m vMask1, vMask2
    add pSrc, pSrc, iCurrentVL
    vse8.v vStr1, (pDstPtr), vMask1.t
    add pDstPtr, pDstPtr, iCurrentVL
    bltz iActiveElemPos, L(strcpy_loop)

    ret

.option pop
END(__strcat_rvv)
libc_hidden_builtin_def (__strcat_rvv)
