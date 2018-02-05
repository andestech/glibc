#include <libc-symbols.h>
#include <sysdep.h>

#ifndef PREINIT_FUNCTION
# define PREINIT_FUNCTION __gmon_start__
#endif

#ifndef PREINIT_FUNCTION_WEAK
# define PREINIT_FUNCTION_WEAK 1
#endif

#if PREINIT_FUNCTION_WEAK
        weak_extern (PREINIT_FUNCTION)
#else
        .hidden PREINIT_FUNCTION
#endif
	.abi_2
	.pic
	.section .init
	.align  2
	.globl	_init
	.type	_init, @function
_init:
	smw.adm	$sp, [$sp], $sp, 6
	sethi	$gp, hi20(_GLOBAL_OFFSET_TABLE_-8)
	ori	$gp, $gp, lo12(_GLOBAL_OFFSET_TABLE_-4)
	mfusr	$ta, $pc
	add	$gp, $ta, $gp

	la	$r0, PREINIT_FUNCTION@GOT
	jralnez	$r0

	.section .fini
	.align	2
	.globl	_fini
	.type	_fini, @function
_fini:
	smw.adm	$sp, [$sp], $sp, 6
	sethi	$gp, hi20(_GLOBAL_OFFSET_TABLE_-8)
	ori	$gp, $gp, lo12(_GLOBAL_OFFSET_TABLE_-4)
	mfusr	$ta, $pc
	add	$gp, $ta, $gp
