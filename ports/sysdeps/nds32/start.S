/* Startup code for NDS32 & ELF
   Copyright (C) 1995, 1996, 1997, 1998, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* This is the canonical entry point, usually the first thing in the text
   segment.

	Note that the code in the .init section has already been run.
	This includes _init and _libc_init


	At this entry point, most registers' values are unspecified, except:

   r5		Contains a function pointer to be registered with `atexit'.
		This is how the dynamic linker arranges to have DT_FINI
		functions called for shared libraries that have been loaded
		before this code runs.

   sp		The stack contains the arguments and environment:
		0(sp)			argc
		4(sp)			argv[0]
		...
		(4*argc)(sp)		NULL
		(4*(argc+1))(sp)	envp[0]
		...
					NULL
*/


	.text
	.align	4
	.globl _start
	.type  _start, @function
	
#ifdef SHARED
.pic
1:
	ret
#endif	
	
_start:
	movi  $fp,	0			! clear FP
	lwi	$r1,	[$sp + 0]	! r1 = argc
	addi	$r2,	$sp,	4	! r2 = argv
	
	/* align sp to 8-byte boundary */
	movi	$r0,	-8
	and	$sp,	$sp,	$r0

	addi	$r6,	$sp,	0	! r6 = stack top
	
#ifdef SHARED
	/* set gp register */
#ifdef __NDS32_N1213_43U1H__
	jal	1b
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_)
	ori	$gp,	$gp, LO12(_GLOBAL_OFFSET_TABLE_ + 4)
	add	$gp,	$lp,	$gp
#else
#ifdef __NDS32_BASELINE_V3__
        sethi   $gp, HI20(_GLOBAL_OFFSET_TABLE_ -  8)
        ori     $gp, $gp, LO12(_GLOBAL_OFFSET_TABLE_ - 4)
        add5.pc  $gp
#else
        sethi   $gp, hi20(_GLOBAL_OFFSET_TABLE_ - 8)
        ori     $gp, $gp, lo12(_GLOBAL_OFFSET_TABLE_ - 4)
        mfusr   $ta, $pc
        add     $gp, $ta, $gp
#endif
#endif
	
	la		$r3,	__libc_csu_init@GOTOFF
	la		$r4,	__libc_csu_fini@GOTOFF
	la		$r0,	main@GOT
	
	/* push everything to stack, r5 is rtld_fini and r7 is garbage */
	pushm	$r0,	$r7
	
	/* now start it up */
	bal	__libc_start_main@PLT
	
	/* should never get here */
	bal	abort@PLT
#else
	la    $gp, _SDA_BASE_ ! init GP for small data access
	
	la		$r3,	__libc_csu_init
	la		$r4,	__libc_csu_fini
	la		$r0,	main
	
	/* push everything to stack, r5 is rtld_fini and r7 is garbage */
	pushm	$r0,	$r7
	
	/* now start it up */
	bal	__libc_start_main
	
	/* should never get here */
	bal	abort
#endif

	ret
	
/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long	0
	.weak data_start
	data_start = __data_start
