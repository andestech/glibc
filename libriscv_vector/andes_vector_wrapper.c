#include <stdio.h>
#include <string.h>

/* Declare the assembly functions */
extern void *__memchr_rvv (void const *s, int c_in, size_t n);
extern int __memcmp_rvv (const void *s1, const void *s2, size_t len);
extern int __memcmpeq_rvv (const void *s1, const void *s2, size_t len);
extern void *__memcpy_rvv (void *dest, const void *src, size_t len);
extern void *__memmove_rvv (void *dest, const void *src, size_t len);
extern void *__memset_rvv (void *dest, int c, size_t n);
extern char *__strcat_rvv (char *dest, const char *src);
extern char *__strchr_rvv (const char *s, int c_in);
extern int __strcmp_rvv (const char *p1, const char *p2);
extern char *__strcpy_rvv (char *dest, const char *src);
extern size_t __strlen_rvv (const char *str);
extern char *__strncat_rvv (char *s1, const char *s2, size_t n);
extern int __strncmp_rvv (const char *p1, const char *p2, size_t n);
extern char *__strncpy_rvv (char *s1, const char *s2, size_t n);
extern size_t __strnlen_rvv (const char *str, size_t maxlen);

void *memchr(void const *s, int c_in, size_t n)
{
	/* Call our RISC-V assembly implementation */
	return __memchr_rvv(s, c_in, n);
}

int memcmp (const void *s1, const void *s2, size_t len)
{
	/* Call our RISC-V assembly implementation */
	return __memcmp_rvv(s1, s2, len);
}

int memcmpeq (const void *s1, const void *s2, size_t len)
{
	/* Call our RISC-V assembly implementation */
	return __memcmpeq_rvv(s1, s2, len);
}

void *memcpy (void *dest, const void *src, size_t len)
{
	/* Call our RISC-V assembly implementation */
	return __memcpy_rvv(dest, src, len);
}

void *memmove (void *dest, const void *src, size_t len)
{
	/* Call our RISC-V assembly implementation */
	return __memmove_rvv(dest, src, len);
}

void *memset(void* dest, int c, size_t n)
{
	/* Call our RISC-V assembly implementation */
	return __memset_rvv(dest, c, n);
}

char *strcat (char *dest, const char *src)
{
	/* Call our RISC-V assembly implementation */
	return __strcat_rvv(dest, src);
}

char *strchr (const char *s, int c_in)
{
	/* Call our RISC-V assembly implementation */
	return __strchr_rvv(s, c_in);
}

int strcmp (const char *p1, const char *p2)
{
	/* Call our RISC-V assembly implementation */
	return __strcmp_rvv(p1, p2);
}

char *strcpy (char *dest, const char *src)
{
	/* Call our RISC-V assembly implementation */
	return __strcpy_rvv(dest, src);
}

size_t strlen (const char *str)
{
	/* Call our RISC-V assembly implementation */
	return __strlen_rvv(str);
}

char *strncat (char *s1, const char *s2, size_t n)
{
	/* Call our RISC-V assembly implementation */
	return __strncat_rvv(s1, s2, n);
}

int strncmp (const char *p1, const char *p2, size_t n)
{
	/* Call our RISC-V assembly implementation */
	return __strncmp_rvv(p1, p2, n);
}

char *strncpy (char *s1, const char *s2, size_t n)
{
	/* Call our RISC-V assembly implementation */
	return __strncpy_rvv(s1, s2, n);
}

size_t strnlen (const char *str, size_t maxlen)
{
	/* Call our RISC-V assembly implementation */
	return __strnlen_rvv(str, maxlen);
}