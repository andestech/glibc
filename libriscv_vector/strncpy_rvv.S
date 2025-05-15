/* RVV versions strncpy.  RISC-V version.
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
#define iLength a2
#define pDstPtr a3

#define iVL a4
#define iCurrentVL a5
#define iActiveElemPos a6
#define iTemp a7

#define ELEM_LMUL_SETTING m1
#define vMask1 v0
#define vMask2 v1
#define ZERO_FILL_ELEM_LMUL_SETTING m8
#define vStr1 v8
#define vStr2 v16

ENTRY(__strncpy_rvv)
.option push
.option arch,+v

    mv pDstPtr, pDst

    /* Copy pSrc to pDstPtr.  */
L(strcpy_loop):
    vsetvli zero, iLength, e8, ELEM_LMUL_SETTING, ta, ma
    vle8ff.v vStr1, (pSrc)
    vmseq.vx vMask2, vStr1, zero
    csrr iCurrentVL, vl
    vfirst.m iActiveElemPos, vMask2
    vmsif.m vMask1, vMask2
    add pSrc, pSrc, iCurrentVL
    sub iLength, iLength, iCurrentVL
    vse8.v vStr1, (pDstPtr), vMask1.t
    add pDstPtr, pDstPtr, iCurrentVL
    bgez iActiveElemPos, L(fill_zero)
    bnez iLength, L(strcpy_loop)
    ret

    /* Fill the tail zero.  */
L(fill_zero):
    /* We already copy the `\0` to dst. But we use `vfirst.m` to
       get the `index` of `\0` position. We need to adjust `-1`
       to get the correct remaining iLength for zero filling.  */
    sub iTemp, iCurrentVL, iActiveElemPos
    addi iTemp, iTemp, -1
    add iLength, iLength, iTemp
    /* Have an earily return for `strlen(src) + 1 == count` case.  */
    bnez iLength, 1f
    ret
1:
    sub pDstPtr, pDstPtr, iTemp
    vsetvli zero, iLength, e8, ZERO_FILL_ELEM_LMUL_SETTING, ta, ma
    vmv.v.x vStr2, zero

L(fill_zero_loop):
    vsetvli iVL, iLength, e8, ZERO_FILL_ELEM_LMUL_SETTING, ta, ma
    vse8.v vStr2, (pDstPtr)
    sub iLength, iLength, iVL
    add pDstPtr, pDstPtr, iVL
    bnez iLength, L(fill_zero_loop)

    ret

.option pop
END(__strncpy_rvv)
libc_hidden_builtin_def (__strncpy_rvv)
