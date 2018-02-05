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
	result = INLINE_SYSCALL_NCS(__sysno,6,arg1,arg2,arg3,arg4,arg5,arg6);
	return result;
}
