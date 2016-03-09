/* Copyright (C) 2013-2014 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

/* We need a special version of the `mcount' function because it has
   to preserve more registers than your usual function.  */

static void __mcount_internal (u_long frompc, u_long selfpc) __attribute_used__;

#define _MCOUNT_DECL(frompc, selfpc) \
static void __mcount_internal (u_long frompc, u_long selfpc)

#if defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__)
#define NDS32_STACK_PUSH_STR
#define NDS32_STACK_POP_STR
#else /* !(defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__))  */
#define NDS32_STACK_PUSH_STR "addi $sp, $sp, -24"
#define NDS32_STACK_POP_STR "addi $sp, $sp, 24"
#endif /* !(defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__))  */

#define MCOUNT					\
asm (						\
"\n"						\
"	.text\n"				\
"	.align	2\n"				\
"	.globl	_mcount\n"			\
"	.type	_mcount, @function\n"		\
"_mcount:\n"					\
"	smw.adm	$r0, [$sp], $r6, 0x2\n"		\
"	move	$r1, $lp\n"			\
"	lwi	$r0, [$sp+36]\n"		\
"	"NDS32_STACK_PUSH_STR"\n"		\
"	bal	__mcount_internal\n"		\
"	"NDS32_STACK_POP_STR"\n"		\
"	lmw.bim	$r0, [$sp], $r6, 0x2\n"		\
"	ret\n"					\
"	.size _mcount, .-_mcount\n"		\
);


