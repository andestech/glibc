#ifndef _LINUX_NDS32_SYSDEP_H
#define _LINUX_NDS32_SYSDEP_H 1

#include <sysdeps/unix/sysv/linux/generic/sysdep.h>
#include <sysdeps/nds32/sysdep.h>

/* Defines RTLD_PRIVATE_ERRNO and USE_DL_SYSINFO.  */
#include <dl-sysdep.h>

#include <tls.h>

/* In order to get __set_errno() definition in INLINE_SYSCALL.  */
#ifndef __ASSEMBLER__
#include <errno.h>
#endif

#undef SYS_ify
#define SYS_ify(syscall_name) __NR_##syscall_name

#ifdef __ASSEMBLER__

/* Child's $SP for clone syscall will be $r1, 
 * so the pushed $r7, $r8 will not be in cloned child's stack.
 */
#define __do_syscall(syscall_name)		\
  movi	$r15,	SYS_ify(syscall_name);		\
  syscall	0x0;

#define ADJUST_GP				\
	smw.adm $sp,[$sp],$sp,#0x6;		\
	cfi_startproc;				\
	.cfi_adjust_cfa_offset 8;		\
	.cfi_rel_offset gp, 0;			\
	.cfi_rel_offset lp, 4;			\
	mfusr	$r15,	$PC;			\
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_ + 4);	\
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_ + 8);\
	add	$gp,	$r15,	$gp;

#define RECOVER_GP				\
	lmw.bim $sp,[$sp],$sp,#0x6;		\
	.cfi_adjust_cfa_offset -8;		\
	.cfi_restore gp;			\
	.cfi_restore lp;			\
	cfi_endproc;

# undef PSEUDO
#define PSEUDO(name, syscall_name, args)	\
  .align 2;					\
	ENTRY(name);                          	\
	__do_syscall(syscall_name);            	\
	bgez $r0, 2f;				\
	sltsi	$r1, $r0, -4096;		\
	beqz	$r1, SYSCALL_ERROR_LABEL;	\
  2:

#undef PSEUDO_END
#define PSEUDO_END(sym)				\
	cfi_endproc;				\
	SYSCALL_ERROR_HANDLER			\
	cfi_startproc;				\
	END(sym)

#define PSEUDO_NOERRNO(name, syscall_name, args) \
  ENTRY(name);                                   \
  __do_syscall(syscall_name);

#undef PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(name)                        \
  END (name)

# undef PSEUDO_ERRVAL
#define PSEUDO_ERRVAL(name, syscall_name, args) \
  PSEUDO_NOERRNO(name, syscall_name, args)	\
  neg $r0, $r0;

#undef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym) END(sym)

#define ret_ERRVAL ret

#define ret_NOERRNO ret

# if !IS_IN (libc)
#  if RTLD_PRIVATE_ERRNO
#  define SYSCALL_ERROR_HANDLER				\
SYSCALL_ERROR_LABEL:					\
	ADJUST_GP					\
	la	$r1,	(rtld_errno@GOT);		\
	neg	$r0, 	$r0;				\
	sw 	$r0,	[$r1];				\
	li	$r0, 	-1;				\
	RECOVER_GP					\
	ret;
#  else /*RTLD_PRIVATE_ERRNO*/
#   ifdef PIC
#   define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	ADJUST_GP					\
	neg	$r0,	$r0;				\
        sethi   $r15, 	hi20(errno@GOTTPOFF);		\
        ori     $r15, 	$r15, lo12(errno@GOTTPOFF);	\
	lw	$r15,	[$r15 + $gp];			\
	sw	$r0,	[$r25 + $r15];			\
	li	$r0,	-1;				\
	RECOVER_GP					\
	ret;
#   else
#   define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	neg	$r0,	$r0;				\
        sethi 	$r15, 	hi20(errno@GOTTPOFF);		\
        lwi 	$r15, 	[$r15 + lo12(errno@GOTTPOFF)];	\
	sw	$r0,	[$r25 + $r15];			\
	li	$r0,	-1;				\
	ret;
#   endif
#  endif
# else
#  ifdef PIC
#  define SYSCALL_ERROR_HANDLER				\
SYSCALL_ERROR_LABEL:					\
	ADJUST_GP					\
	bal	__syscall_error;			\
	RECOVER_GP					\
	ret;
#  else
#  define SYSCALL_ERROR_HANDLER				\
SYSCALL_ERROR_LABEL:					\
	b	__syscall_error;
#  endif
# endif

#else /* ! __ASSEMBLER__  */

# define HAVE_GETTIMEOFDAY_VSYSCALL	1
# define HAVE_CLOCK_GETRES_VSYSCALL	1
# define HAVE_CLOCK_GETTIME_VSYSCALL	1

#define __issue_syscall(syscall_name)                   \
"       syscall  0x0;\n"

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)                        \
  ({                                                             \
     INTERNAL_SYSCALL_DECL (err);                                \
     long result_var = INTERNAL_SYSCALL (name, err, nr, args);   \
     if (INTERNAL_SYSCALL_ERROR_P (result_var, err))             \
       {                                                         \
         __set_errno (INTERNAL_SYSCALL_ERRNO (result_var, err)); \
			result_var = -1 ;			 \
       }                                                         \
     result_var;                                                 \
  })

#undef INLINE_SYSCALL_NCS
#define INLINE_SYSCALL_NCS(name, nr, args...)                        \
  ({                                                             \
     INTERNAL_SYSCALL_DECL (err);                                \
     long result_var = INTERNAL_SYSCALL_NCS (name, err, nr, args);   \
     if (INTERNAL_SYSCALL_ERROR_P (result_var, err))             \
       {                                                         \
         __set_errno (INTERNAL_SYSCALL_ERRNO (result_var, err)); \
			result_var = -1 ;			 \
       }                                                         \
     result_var;                                                 \
  })
#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#undef INTERNAL_SYSCALL_ERROR_P
/* #define INTERNAL_SYSCALL_ERROR_P(val, err) (((long) (val)) < 0) */
#define INTERNAL_SYSCALL_ERROR_P(val, err) ((unsigned int) (val) >= 0xfffff001u)

#undef INTERNAL_SYSCALL_ERRNO
/* #define INTERNAL_SYSCALL_ERRNO(val, err) (err) */
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) internal_syscall##nr(__NR_##name, err, args)

/*
   The _NCS variant allows non-constant syscall numbers but it is not
   possible to use more than four parameters.  
*/

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) internal_syscall##nr(name, err, args)

/* We negate the return value because Linux will return a negated errno. */
#define __syscall_result(err, res)	                        \
  do                                                            \
    {                                                           \
      if ((unsigned long) res >= (unsigned long) (- (124 + 1))) \
      {                                                         \
          err = - (res);                                        \
          res = -1;                                             \
      }                                                         \
    }                                                           \
  while (0)

#define internal_syscall0(name, err, dummy...)                   \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
       __res;							 \
  })

#define internal_syscall1(name, err, arg1)                       \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall2(name, err, arg1, arg2)                 \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall3(name, err, arg1, arg2, arg3)           \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall4(name, err, arg1, arg2, arg3, arg4)     \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall5(name, err, arg1, arg2, arg3, arg4, arg5) \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       register long __arg5 asm ("$r4") = (long) (arg5);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       , "r" (__arg5)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall6(name, err, arg1, arg2, arg3, arg4, arg5, arg6) \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       register long __arg5 asm ("$r4") = (long) (arg5);         \
       register long __arg6 asm ("$r5") = (long) (arg6);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__num)         /* input operands  */              \
       , "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       , "r" (__arg5)         /* input operands  */              \
       , "r" (__arg6)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define __SYSCALL_CLOBBERS "$lp", "memory"

#endif /* ! __ASSEMBLER__  */

#define PTR_MANGLE(var) (void) (var)
#define PTR_DEMANGLE(var) (void) (var)

#endif /* _LINUX_NDS32_SYSDEP_H  */
