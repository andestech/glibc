#ifndef _LINUX_NDS32_SYSDEP_H
#define _LINUX_NDS32_SYSDEP_H 1

#include <sysdeps/unix/nds32/sysdep.h>
#include <tls.h>

#define        LIB_SYSCALL    __NR_syscall

#undef SYS_ify
#ifdef __STDC__
#define SYS_ify(syscall_name) __NR_##syscall_name
#else /* ! __STDC__  */
#define SYS_ify(syscall_name) __NR_/**/syscall_name
#endif /* ! __STDC__  */

#ifdef __ASSEMBLER__

/* Child's $SP for clone syscall will be $r1, 
 * so the pushed $r7, $r8 will not be in cloned child's stack.
 */
#ifdef NDS_ABI_V1
#define __do_syscall(syscall_name)		\
  pushm	$r7, $r8;				\
  li	$r7, SYS_ify(syscall_name);  	\
  ori	$r7, $r7, lo12(SYS_ify(syscall_name)); 	\
  syscall	LIB_SYSCALL;			\
  li  $r0, SYS_ify(clone);       	\
  bne	$r0, $r7, 1f;				\
  beqz	$r5, 2f;				\
1:						\
  popm	$r7, $r8;				\
2:
#else
#define __do_syscall(syscall_name)		\
  syscall	SYS_ify(syscall_name);
#endif

/* We negate the return value because Linux will return a negated errno. */
#ifdef PIC
#ifdef __NDS32_N1213_43U1H__
#define PSEUDO(name, syscall_name, args)	\
  .pic;										\
  .align 2;					\
  1:	ret;	\
  99:	addi	$r2,	$lp,	0;	\
	jal	1b;	\
	sethi	$r1,	hi20(_GLOBAL_OFFSET_TABLE_);	\
	ori	$r1,	$r1,	lo12(_GLOBAL_OFFSET_TABLE_+4);	\
	add	$r1,	$lp,	$r1;	\
	addi	$lp,	$r2,	0;	\
	sethi $r15, hi20(SYSCALL_ERROR@PLT);	\
	ori	$r15,	$r15, lo12(SYSCALL_ERROR@PLT);	\
	add	$r15, $r15, $r1;	\
	jr		$r15;	\
	nop;                                   	\
	ENTRY(name);                          	\
	__do_syscall(syscall_name);            	\
	bgez $r0, 2f;				\
	sltsi	$r1, $r0, -4096;		\
	beqz	$r1, 99b;			\
  2:
#else
#define PSEUDO(name, syscall_name, args)	\
  .pic;						\
  .align 2;					\
  99:	pushm	$gp,	$lp;			\
	mfusr	$r15,	$PC;			\
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_ + 4);	\
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_ + 8);\
	add	$gp,	$r15,	$gp;		\
	sethi	$r15,	hi20(SYSCALL_ERROR@PLT);		\
	ori	$r15,	$r15,	lo12(SYSCALL_ERROR@PLT);	\
	add	$r15,	$r15,	$gp;		\
	jral		$r15;			\
	popm	$gp,	$lp;			\
	ret;					\
	nop;                                   	\
	ENTRY(name);                          	\
	__do_syscall(syscall_name);            	\
	bgez $r0, 2f;				\
	sltsi	$r1, $r0, -4096;		\
	beqz	$r1, 99b;			\
  2:
#endif
#else
#define PSEUDO(name, syscall_name, args)	\
  .align 2;					\
  99:	j SYSCALL_ERROR;                  	\
	nop;                                   	\
	ENTRY(name);                          	\
	__do_syscall(syscall_name);            	\
        bgez $r0, 2f;                           \
        sltsi   $r1, $r0, -4096;                \
        beqz    $r1, 99b;                       \
  2:
#endif

#define PSEUDO_NOERRNO(name, syscall_name, args) \
  ENTRY(name);                                   \
  __do_syscall(syscall_name);

#undef PSEUDO_END
#define PSEUDO_END(sym)				\
	SYSCALL_ERROR_HANDLER			\
	END(sym)

#undef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym) END(sym)

#define PSEUDO_ERRVAL(name, syscall_name, args) PSEUDO_NOERRNO(name, syscall_name, args)

#define ret_ERRVAL ret

#define ret_NOERRNO ret

#if defined(__NDS32_ABI_2__) || defined(__NDS32_ABI_2FP_PLUS__)
# define STACK_PUSH(count) addi $sp, $sp, count
# define STACK_POP(count) addi $sp, $sp, count
#else
# define STACK_PUSH(count) addi $sp, $sp, -24+(count)
# define STACK_POP(count) addi $sp, $sp, 24+(count)
#endif

#if NOT_IN_libc
#define SYSCALL_ERROR __local_syscall_error
# ifdef PIC
#  ifdef __NDS32_N1213_43U1H__
#define SYSCALL_ERROR_HANDLER				\
__local_syscall_error:	pushm	$gp, $lp;				\
	jal	1f;	\
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_);	\
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_+4);	\
	add	$gp,	$gp,	$lp;	\
	neg	$r0, $r0;	\
	push	$r0;				\
	STACK_PUSH(-4); \
	bal	C_SYMBOL_NAME(__errno_location@PLT);	\
	STACK_POP(4); \
	pop	$r1;			\
	swi	$r1, [$r0];				\
	li		$r0, -1;				\
	popm	$gp, $lp;				\
1: \
   ret;
#  else
#define SYSCALL_ERROR_HANDLER				\
__local_syscall_error:	pushm	$gp, $lp;				\
	mfusr $r15, $PC;	\
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_+4);	\
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_+8);	\
	add	$gp,	$gp,	$r15;	\
	neg	$r0, $r0;	\
	push	$r0;				\
	STACK_PUSH(-4); \
	bal	C_SYMBOL_NAME(__errno_location@PLT);	\
	STACK_POP(4); \
	pop	$r1;			\
	swi	$r1, [$r0];				\
	li		$r0, -1;				\
	popm	$gp, $lp;				\
1: \
   ret;
#  endif
# else
#define SYSCALL_ERROR_HANDLER	\
__local_syscall_error:	push	$lp;				\
	neg	$r0, $r0;	\
	push	$r0;				\
	STACK_PUSH(0); \
	bal	C_SYMBOL_NAME(__errno_location);	\
	STACK_POP(0); \
	pop	$r1;			\
	swi	$r1, [$r0];				\
	li		$r0, -1;				\
	pop	$lp;				\
	ret;
# endif

#undef PUSH_STACK
#undef POP_STACK
/* arm
#define SYSCALL_ERROR_HANDLER				\
__local_syscall_error:					\
	str	lr, [sp, #-4]				\
	str	r0, [sp, #-4]				\
	bl	PLTJMP(C_SYMBOL_NAME(__errno_location))	\
	ldr	r1, [sp], #4				\
	rsb	r1, r1, #0				\
	str	r1, [r0]				\
	mvn	r0, #0					\
	ldr	pc, [sp], #4
*/
#else
#define SYSCALL_ERROR_HANDLER	/* Nothing here; code in sysdep.S is used. */
#define SYSCALL_ERROR __syscall_error
#endif

#else /* ! __ASSEMBLER__  */

/*
#define __issue_syscall(syscall_name)			\
	push	r7					\
	sethi r7, hi20(SYS_ify(syscall_name));		\
	ori   r7, r7, lo12(SYS_ify(syscall_name));	\
	syscall       LIB_SYSCALL;			\
	pop	r7
*/

#define X(x) #x
#define Y(x) X(x)

#define __issue_syscall(syscall_name)                   \
"       syscall  "  Y(syscall_name) ";                    \n"

#ifndef __set_errno
#include <errno.h>
// # define __set_errno(Val) errno = (Val)
#endif

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
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) internal_syscall_ncs##nr(name, err, args)

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
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       :							 \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
       __res;							 \
  })

#define internal_syscall1(name, err, arg1)                       \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall2(name, err, arg1, arg2)                 \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall3(name, err, arg1, arg2, arg3)           \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall4(name, err, arg1, arg2, arg3, arg4)     \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall5(name, err, arg1, arg2, arg3, arg4, arg5) \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       register long __arg5 asm ("$r4") = (long) (arg5);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
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
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       register long __arg5 asm ("$r4") = (long) (arg5);         \
       register long __arg6 asm ("$r5") = (long) (arg6);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       , "r" (__arg5)         /* input operands  */              \
       , "r" (__arg6)         /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall_ncs0(name, err, dummy...)               \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __no   asm ("$r0") = (long) (name);         \
       __asm__ volatile (                                        \
       __issue_syscall (LIB_SYSCALL)                             \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__no)           /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
       __res;							 \
  })

#define internal_syscall_ncs1(name, err, arg1)                   \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __no   asm ("$r0") = (long) (name);         \
       register long __arg1 asm ("$r1") = (long) (arg1);         \
       __asm__ volatile (                                        \
       __issue_syscall (LIB_SYSCALL)                             \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__no)           /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall_ncs2(name, err, arg1, arg2)             \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __no   asm ("$r0") = (long) (name);         \
       register long __arg1 asm ("$r1") = (long) (arg1);         \
       register long __arg2 asm ("$r2") = (long) (arg2);         \
       __asm__ volatile (                                        \
       __issue_syscall (LIB_SYSCALL)                             \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__no)           /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall_ncs3(name, err, arg1, arg2, arg3)       \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __no   asm ("$r0") = (long) (name);         \
       register long __arg1 asm ("$r1") = (long) (arg1);         \
       register long __arg2 asm ("$r2") = (long) (arg2);         \
       register long __arg3 asm ("$r3") = (long) (arg3);         \
       __asm__ volatile (                                        \
       __issue_syscall (LIB_SYSCALL)                             \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__no)           /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define internal_syscall_ncs4(name, err, arg1, arg2, arg3, arg4) \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __no   asm ("$r0") = (long) (name);         \
       register long __arg1 asm ("$r1") = (long) (arg1);         \
       register long __arg2 asm ("$r2") = (long) (arg2);         \
       register long __arg3 asm ("$r3") = (long) (arg3);         \
       register long __arg4 asm ("$r4") = (long) (arg4);         \
       __asm__ volatile (                                        \
       __issue_syscall (LIB_SYSCALL)                             \
       : "=r" (__res)         /* output operands  */             \
       : "r" (__arg1)         /* input operands  */              \
       , "r" (__arg2)         /* input operands  */              \
       , "r" (__arg3)         /* input operands  */              \
       , "r" (__arg4)         /* input operands  */              \
       , "r" (__no)           /* input operands  */              \
       : __SYSCALL_CLOBBERS); /* list of clobbered registers  */ \
        __res;                                                   \
  })

#define __SYSCALL_CLOBBERS "$lp", "memory"

#endif /* ! __ASSEMBLER__  */

#define PTR_MANGLE(var) (void) (var)
#define PTR_DEMANGLE(var) (void) (var)

#endif /* _LINUX_NDS32_SYSDEP_H  */
