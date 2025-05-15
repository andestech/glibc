/* RVV versions strncmp.  RISC-V version.
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
   <http:/*www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <sys/asm.h>

#define iResult a0

#define pStr1 a0
#define pStr2 a1
#define iLength a2

#define iVL a3
#define iTemp1 a4
#define iTemp2 a5

#define ELEM_LMUL_SETTING m1
#define vStr1 v0
#define vStr2 v4
#define vMask1 v8
#define vMask2 v9

ENTRY(__strncmp_rvv)
.option push
.option arch,+v

    beqz iLength, L(zero_length)

L(loop):
    vsetvli zero, iLength, e8, ELEM_LMUL_SETTING, ta, ma

    vle8ff.v vStr1, (pStr1)
    /* vStr1[i] == 0.  */
    vmseq.vx vMask1, vStr1, zero

    vle8ff.v vStr2, (pStr2)
    /* vStr1[i] != vStr2[i].  */
    vmsne.vv vMask2, vStr1, vStr2

    csrr iVL, vl

    /* r = mask1 | mask2
       We could use vfirst.m to get the first zero char or the
       first different char between str1 and str2.  */
    vmor.mm vMask1, vMask1, vMask2

    sub iLength, iLength, iVL

    vfirst.m iTemp1, vMask1

    bgez iTemp1, L(end_loop)

    add pStr1, pStr1, iVL
    add pStr2, pStr2, iVL
    bnez iLength, L(loop)
L(end_loop):

    add pStr1, pStr1, iTemp1
    add pStr2, pStr2, iTemp1
    lbu iTemp1, 0(pStr1)
    lbu iTemp2, 0(pStr2)

    sub iResult, iTemp1, iTemp2
    ret

L(zero_length):
    li iResult, 0
    ret

.option pop
END(__strncmp_rvv)
libc_hidden_builtin_def (__strncmp_rvv)
