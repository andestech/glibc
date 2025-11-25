/* RISC-V multiarch strchr, V-extension version.
   Copyright (C) 2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Nick Knight <nick.knight@sifive.com>.

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




ENTRY(__strchr_rvv)
.option push
.option arch,+v

0:
    vsetvli t0, zero, e8, m8, ta, ma
    vle8ff.v v0, (a0)
    vmseq.vi v8, v0, 0
    vmseq.vx v9, v0, a1
    vfirst.m a2, v8 /* first occurrence of \0 */
    vfirst.m a3, v9 /* first occurrence of ch */
    addi a4, a3, 1
    seqz a4, a4
    sltu a5, a2, a3
    or a4, a4, a5
    beqz a4, 1f /* Found ch, not preceded by \0? */
    li a6, -1
    csrr a5, vl
    add a0, a0, a5
    beq a2, a6, 0b /* Didn't find \0? */
    li a0, 0
    ret
1:
    add a0, a0, a3
    ret

.option pop
END(__strchr_rvv)
weak_alias (__strchr_rvv, index)
libc_hidden_builtin_def (__strchr_rvv)


