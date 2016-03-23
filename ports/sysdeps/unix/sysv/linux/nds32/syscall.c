#include <errno.h>
#include <stdarg.h>
#include <sysdep.h>
long int syscall (long int __sysno, ...)
{

	int result;
	unsigned long arg1,arg2,arg3,arg4,arg5,arg6;
	va_list arg;
	va_start (arg, __sysno);
	arg1 = va_arg (arg, unsigned long);
	arg2 = va_arg (arg, unsigned long);
	arg3 = va_arg (arg, unsigned long);
	arg4 = va_arg (arg, unsigned long);
	arg5 = va_arg (arg, unsigned long);
	arg6 = va_arg (arg, unsigned long);
	va_end (arg);
        __asm__ volatile ( "" ::: "memory" );
	__asm__ volatile(
	"addi10.sp\t  #-4\n\t"
	".cfi_adjust_cfa_offset 4\n\t"
	"push\t	%0\n\t"
	".cfi_adjust_cfa_offset 4\n\t"
	:
	:"r"(arg6)
	);
	result = INLINE_SYSCALL(syscall,6,__sysno,arg1,arg2,arg3,arg4,arg5);
	__asm__ volatile(
	"pop\t	%0\n\t"
	".cfi_adjust_cfa_offset -4\n\t"
	"addi10.sp\t  #4\n\t"
	".cfi_adjust_cfa_offset -4\n\t"
	:"=r"(arg6)
	);
	return result;
}
