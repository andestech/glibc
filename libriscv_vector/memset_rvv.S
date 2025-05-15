/* RVV versions memset.  RISC-V version.
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
#define iValue a1
#define iNum a2

#define iVL a3
#define iTemp a4
#define pDstPtr a5

#define ELEM_LMUL_SETTING m8
#define vData v0

ENTRY(__memset_rvv)
.option push
.attribute unaligned_access, 1
.option arch,+v

    mv pDstPtr, pDst

    vsetvli iVL, iNum, e8, ELEM_LMUL_SETTING, ta, ma
    vmv.v.x vData, iValue

L(loop):
    vse8.v vData, (pDstPtr)
    sub iNum, iNum, iVL
    add pDstPtr, pDstPtr, iVL
    vsetvli iVL, iNum, e8, ELEM_LMUL_SETTING, ta, ma
    bnez iNum, L(loop)

    ret

.option pop
END(__memset_rvv)
libc_hidden_builtin_def (__memset_rvv)
