#include <string.h>
#include <stdint.h>

#undef strcpy

#ifndef STRCPY
# define STRCPY strcpy
#endif

#ifdef PREFER_SIZE_OVER_SPEED
#undef PREFER_SIZE_OVER_SPEED
#endif

#ifdef __OPTIMIZE_SIZE__
#undef __OPTIMIZE_SIZE__
#endif

static __inline unsigned long __libc_detect_null(unsigned long w)
{
#ifdef __nds_v5
  unsigned long r;
  __asm__ volatile("ffb %0, %1, zero" : "=r"(r) : "r"(w));
  return r != 0;
#else
  unsigned long mask = 0x7f7f7f7f;
  if (sizeof (long) == 8)
    mask = ((mask << 16) << 16) | mask;
  return ~(((w & mask) + mask) | w | mask);
#endif
}

__attribute__((optimize("-O2")))
char *strcpy(char *dst, const char *src)
{
  char *dst0 = dst;

#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
  int misaligned = ((uintptr_t)dst | (uintptr_t)src) & (sizeof (long)-1);
  if (__builtin_expect(!misaligned, 1))
    {
      long *ldst = (long *)dst;
      const long *lsrc = (const long *)src;

      while (!__libc_detect_null(*lsrc))
	*ldst++ = *lsrc++;

      dst = (char *)ldst;
      src = (const char *)lsrc;

      char c0 = src[0];
      char c1 = src[1];
      char c2 = src[2];
      if (!(*dst++ = c0)) return dst0;
      if (!(*dst++ = c1)) return dst0;
      char c3 = src[3];
      if (!(*dst++ = c2)) return dst0;
      if (sizeof (long) == 4) goto out;
      char c4 = src[4];
      if (!(*dst++ = c3)) return dst0;
      char c5 = src[5];
      if (!(*dst++ = c4)) return dst0;
      char c6 = src[6];
      if (!(*dst++ = c5)) return dst0;
      if (!(*dst++ = c6)) return dst0;

out:
      *dst++ = 0;
      return dst0;
    }
#endif /* not PREFER_SIZE_OVER_SPEED */

  char ch;
  do
    {
      ch = *src;
      src++;
      dst++;
      *(dst - 1) = ch;
    } while (ch);

  return dst0;
}
libc_hidden_builtin_def (strcpy)
