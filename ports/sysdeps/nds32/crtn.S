
#if defined(__NDS32_ABI_2FP_PLUS__)

#include <sysdep.h>

	.pic

	.section .init

	lmw.bim	$sp, [$sp], $sp, 6
	.hint_func_args 63
	ret

	.section .fini

	lmw.bim	$sp, [$sp], $sp, 6
	.hint_func_args 63
	ret

	.weak	__gmon_start__

#else

#include <sysdep.h>

	.abi_2
	.pic
	.section .init

	lmw.bim	$sp, [$sp], $sp, 6
	.hint_func_args 63
	ret

	.section .fini

	lmw.bim	$sp, [$sp], $sp, 6
	.hint_func_args 63
	ret

	.weak	__gmon_start__

#endif
