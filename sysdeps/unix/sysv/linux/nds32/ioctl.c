#include <errno.h>
#include <stdarg.h>
#include <sysdep.h>

#include <sys/ioctl.h>
#include <sys/syscall.h>
int ioctl (int __fd, unsigned long int __request, ...);
libc_hidden_proto(ioctl)

int __ioctl (int __fd, unsigned long int __request, ...)
{
	unsigned long arg1;
	va_list arg;
	va_start (arg, __request);
	arg1 = va_arg (arg, unsigned long);
	va_end (arg);
	return INLINE_SYSCALL(ioctl,3,__fd,__request,arg1);
}
weak_alias (__ioctl, ioctl)
libc_hidden_weak (ioctl)
