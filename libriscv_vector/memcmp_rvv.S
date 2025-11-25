/* RVV versions memcmp.  RISC-V version.
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

#define pSrc1 a0
#define pSrc2 a1
#define iNum a2

#define iVL a3
#define iTemp a4
#define iTemp1 a5
#define iTemp2 a6

#define ELEM_LMUL_SETTING m8
#define vData1 v0
#define vData2 v8
#define vMask v16

ENTRY(__memcmp_rvv)
.option push
.option arch,+v
L(loop):
    vsetvli iVL, iNum, e8, ELEM_LMUL_SETTING, ta, ma

    vle8.v vData1, (pSrc1)
    vle8.v vData2, (pSrc2)

    vmsne.vv vMask, vData1, vData2
    sub iNum, iNum, iVL
    vfirst.m iTemp, vMask

    /* Skip the loop if we find the different value between pSrc1 and pSrc2.  */
    bgez iTemp, L(found)

    add pSrc1, pSrc1, iVL
    add pSrc2, pSrc2, iVL

    bnez iNum, L(loop)

    li iResult, 0
    ret

L(found):
    add pSrc1, pSrc1, iTemp
    add pSrc2, pSrc2, iTemp
    lbu iTemp1, 0(pSrc1)
    lbu iTemp2, 0(pSrc2)
    sub iResult, iTemp1, iTemp2
    ret
.option pop
END(__memcmp_rvv)
libc_hidden_builtin_def (__memcmp_rvv)
