/* RVV versions memmove.  RISC-V version.
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
#define iNum a2

#define iVL a3
#define pDstPtr a4
#define pSrcBackwardPtr a5
#define pDstBackwardPtr a6

#define ELEM_LMUL_SETTING m8
#define vData v0

ENTRY(__memmove_rvv)
.option push
.option arch,+v

    mv pDstPtr, pDst

    /* If pSrc is equal or after pDst, all data in pSrc will be loaded before
       overwrited for the overlapping case. We could use faster `forward-copy`.  */
    bgeu pSrc, pDst, L(forward_copy_loop)
    add pSrcBackwardPtr, pSrc, iNum
    add pDstBackwardPtr, pDst, iNum
    /* If pDst inside source data range, we need to use `backward_copy_loop` to
       handle the overlapping issue.  */
    bltu pDst, pSrcBackwardPtr, L(backward_copy_loop)

L(forward_copy_loop):
    vsetvli iVL, iNum, e8, ELEM_LMUL_SETTING, ta, ma

    vle8.v vData, (pSrc)
    sub iNum, iNum, iVL
    add pSrc, pSrc, iVL
    vse8.v vData, (pDstPtr)
    add pDstPtr, pDstPtr, iVL

    bnez iNum, L(forward_copy_loop)
    ret

L(backward_copy_loop):
    vsetvli iVL, iNum, e8, ELEM_LMUL_SETTING, ta, ma

    sub pSrcBackwardPtr, pSrcBackwardPtr, iVL
    vle8.v vData, (pSrcBackwardPtr)
    sub iNum, iNum, iVL
    sub pDstBackwardPtr, pDstBackwardPtr, iVL
    vse8.v vData, (pDstBackwardPtr)
    bnez iNum, L(backward_copy_loop)
    ret

.option pop
END(__memmove_rvv)
libc_hidden_builtin_def (__memmove_rvv)
