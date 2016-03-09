/* PLT trampolines.  NDS32 version.
   Copyright (C) 2013-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <libintl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ldsodefs.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/types.h>
#include "dynamic-link.h"

#if defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__)
# define STACK_PUSH
# define STACK_POP
# define STACK_PUSH_AUDIT
# define STACK_POP_AUDIT
#else
# define STACK_PUSH	"addi $sp, $sp,	-24"
# define STACK_POP	"addi $sp, $sp,	24"
# define STACK_PUSH_AUDIT     "addi $r19, $r19, -24"
# define STACK_POP_AUDIT      "addi $r19, $r19, 24"
#endif

# define TRAMPOLINE_PROLOG_TEMPLATE(tramp_name) \
asm ("\n\
	.text\n\
	.globl "#tramp_name"\n\
	.type "#tramp_name", #function\n\
	.align 4\n\
"#tramp_name":\n\
	! we get called with\n\
	! 	lp contains the return address from this call\n\
	!	r16 contains offset to target reloc entry\n\
	!	r17 contains GOT[1] (identity of taget lib)\n\
	!  ta is GOT[2] (starting address of this function)\n\
\n\
	! save arguments r0 - r5\n\
	smw.adm	$r0,	[$sp],	$r5,	0\n\
\n\
	! push gp, lp\n\
	smw.adm	$sp,	[$sp],	$sp,	6\n\
\n\
");

#ifdef __NDS32_N1213_43U1H__
# define TRAMPOLINE_GP_TEMPLATE \
asm ("\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+8);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+12);\n\
	add	$gp,	$ta,	$gp;\n\
	");
#else
# define TRAMPOLINE_GP_TEMPLATE  \
asm ("\n\
	mfusr	$ta,	$PC\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+4);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+8);\n\
	add	$gp,	$ta,	$gp;\n\
	");
#endif

# define TRAMPOLINE_BODY_TEMPLATE(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli    $r1,    $r16,   2\n\
	slli    $r16,   $r16,   3\n\
	add     $r1,    $r1,    $r16\n\
	addi	$r2,	$lp,	0\n\
\n\
	! call fixup routine\n\
	bal	"#fixup_name"\n\
\n\
	! save the return\n\
	addi	$ta,	$r0,	0\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	lmw.bim	$r0,	[$sp],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	jr		$r15\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");

// PLTENTER & PLTEXIT version
#if 1
#ifdef SHARED
        #define _DL_CALL_PLTEXIT "_dl_call_pltexit@PLT"
#else
        #define _DL_CALL_PLTEXIT "_dl_call_pltexit"
#endif
        
# define TRAMPOLINE_BODY_TEMPLATE_AUDIT(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli	$r1,	$r16,	2\n\
	slli	$r16,	$r16,	3\n\
	add	$r1,	$r1,	$r16\n\
	addi	$r2,	$lp,	0\n\
	addi	$r3,	$sp,	0\n\
	push	$r3\n\
	addi	$r3,	$r3,	-4\n\
	push	$r3\n\
	smw.adm	$r0,	[$sp],	$r1\n\
	xor	$r4,	$r4,	$r4\n\
	push	$r4\n\
	addi	$r4,	$sp,	0\n\
	push	$r3\n\
	push	$r4\n\
\n\
	! call fixup routine\n\
	addi	$sp,	$sp, -16\n\
	bal	"#fixup_name"\n\
	addi	$sp,	$sp, 16\n\
\n\
	pop	$r4\n\
	pop	$r3\n\
	! save the return\n\
	addi	$r15,	$r0,	0\n\
	smw.adm	$r18,	[$sp],	$r20\n\
	move	$r20,	$r3\n\
	lwi	$r18,	[$r4]\n\
	bgez	$r18,	1f\n\
	! adjust sp and reload registers\n\
	lmw.bim	$r18,	[$sp],	$r20\n\
	addi	$sp,	$sp,	20\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	lmw.bim	$r0,	[$sp],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	jr		$r15\n\
1:\n\
	sub	$sp,	$sp,	$r18\n\
	addi	$r19,	$r4,	20\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP_AUDIT"\n\
	lmw.bim	$sp,	[$r19],	$sp,	6\n\
        lmw.bim	$r0,	[$r19],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	smw.adm	$r18,	[$sp],	$r20\n\
	jral	$r15\n\
	lmw.bim	$r18,	[$sp],	$r20\n\
	add	$sp,	$sp,	$r18\n\
	smw.adm	$r0,	[$sp],	$r1\n\
	addi	$r19,	$r20,	-12\n\
	lmw.bim	$r0,	[$r19],	$r1\n\
	move	$r3,	$sp\n\
	! $r3 outregs\n\
	move	$r2,	$r20\n\
	mfusr	$ta,	$PC\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+4);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+8);\n\
	add	$gp,	$ta,	$gp;\n\
	bal	"_DL_CALL_PLTEXIT"\n\
	lmw.bim	$r0,	[$sp],	$r1\n\
	lmw.bim	$r18,	[$sp],	$r20\n\
	addi	$sp,	$sp,	20\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	addi	$sp,	$sp,	8\n\
	lmw.bim	$r2,	[$sp],	$r5,	0\n\
	ret\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");

#endif

#if 0
// PLTENTER version
# define TRAMPOLINE_BODY_TEMPLATE_AUDIT(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli    $r1,    $r16,   2\n\
	slli    $r16,   $r16,   3\n\
	add     $r1,    $r1,    $r16\n\
	addi	$r2,	$lp,	0\n\
	addi	$r3,	$sp,	0\n\
	push	$r3\n\
	addi	$r3,	$r3,	-4\n\
	push	$r0\n\
	addi	$r4,	$sp,	0\n\
\n\
	! call fixup routine\n\
	bal	"#fixup_name"\n\
\n\
	! save the return\n\
	addi	$ta,	$r0,	0\n\
	pop	$r0\n\
	pop	$r3\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	lmw.bim	$r0,	[$sp],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	jr		$r15\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");
#endif

#ifndef PROF
#ifdef SHARED
#define  ELF_MACHINE_RUNTIME_TRAMPOLINE	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup@PLT);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE_AUDIT (_dl_runtime_profile, _dl_profile_fixup@PLT);
#else
#define  ELF_MACHINE_RUNTIME_TRAMPOLINE	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE_AUDIT (_dl_runtime_profile, _dl_profile_fixup);
#endif
#else
#ifdef SHARED
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup@PLT);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_profile, _dl_fixup@PLT);
#else
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_profile, _dl_fixup);
#endif
#endif

ELF_MACHINE_RUNTIME_TRAMPOLINE


