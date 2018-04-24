/* Assembly macros for Andes nds32.
   Copyright (C) 2011-2018
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */


#ifndef _LINUX_NDS32_SYSDEP_H
#define _LINUX_NDS32_SYSDEP_H 1

#include <sysdeps/unix/sysdep.h>
#include <sysdeps/nds32/sysdep.h>
#include <sysdeps/unix/sysv/linux/generic/sysdep.h>

#include <dl-sysdep.h>

#include <tls.h>

/* In order to get __set_errno() definition in INLINE_SYSCALL.  */
#ifndef __ASSEMBLER__
# include <errno.h>
#endif

#undef SYS_ify
#define SYS_ify(syscall_name) __NR_##syscall_name

#ifdef __ASSEMBLER__

# define __do_syscall(syscall_name)		\
  movi	$r15,	SYS_ify (syscall_name);		\
  syscall	0x0;

# define ADJUST_GP				\
	smw.adm $sp,[$sp],$sp,#0x6;		\
	cfi_startproc;				\
	.cfi_adjust_cfa_offset 8;		\
	.cfi_rel_offset gp, 0;			\
	.cfi_rel_offset lp, 4;			\
	GET_GTABLE ($gp)

# define RECOVER_GP				\
	lmw.bim $sp,[$sp],$sp,#0x6;		\
	.cfi_adjust_cfa_offset -8;		\
	.cfi_restore gp;			\
	.cfi_restore lp;			\
	cfi_endproc;

# undef PSEUDO
# define PSEUDO(name, syscall_name, args)	\
  .align 2;					\
	ENTRY(name);                          	\
	__do_syscall (syscall_name);            \
	bgez $r0, 2f;				\
	sltsi	$r1, $r0, -4096;		\
	beqz	$r1, SYSCALL_ERROR_LABEL;	\
  2:

# undef PSEUDO_END
# define PSEUDO_END(sym)			\
	cfi_endproc;				\
	SYSCALL_ERROR_HANDLER			\
	cfi_startproc;				\
	END(sym)

/* Performs a system call, not setting errno.  */
# define PSEUDO_NOERRNO(name, syscall_name, args) \
  ENTRY (name);                                  \
  __do_syscall (syscall_name);

# undef PSEUDO_END_NOERRNO
# define PSEUDO_END_NOERRNO(name)                \
  END (name)

/* Perfroms a system call, returning the error code.  */
# undef PSEUDO_ERRVAL
# define PSEUDO_ERRVAL(name, syscall_name, args) \
  PSEUDO_NOERRNO (name, syscall_name, args)	\
  neg $r0, $r0;

# undef PSEUDO_END_ERRVAL
# define PSEUDO_END_ERRVAL(sym) END (sym)

# define ret_ERRVAL ret

# define ret_NOERRNO ret

# if !IS_IN (libc)
#  if RTLD_PRIVATE_ERRNO
#   define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	ADJUST_GP					\
	la	$r1,	(rtld_errno@GOT);		\
	neg	$r0, 	$r0;				\
	sw 	$r0,	[$r1];				\
	li	$r0, 	-1;				\
	RECOVER_GP					\
	ret;
#  else
#   ifdef PIC
#    define SYSCALL_ERROR_HANDLER			\
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
#    define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	neg	$r0,	$r0;				\
        sethi 	$r15, 	hi20(errno@GOTTPOFF);		\
        lwi 	$r15, 	[$r15 + lo12(errno@GOTTPOFF)];	\
	sw	$r0,	[$r25 + $r15];			\
	li	$r0,	-1;				\
	ret;
#   endif /* !PIC */
#  endif
# else
#  ifdef PIC
#   define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	ADJUST_GP					\
	bal	__syscall_error;			\
	RECOVER_GP					\
	ret;
#  else
#   define SYSCALL_ERROR_HANDLER			\
SYSCALL_ERROR_LABEL:					\
	b	__syscall_error;
#  endif /* !PIC */
# endif /* In LIBC */

#else

/* List of system calls which are supported as vsyscalls.  */
# define HAVE_GETTIMEOFDAY_VSYSCALL	1
# define HAVE_CLOCK_GETRES_VSYSCALL	1
# define HAVE_CLOCK_GETTIME_VSYSCALL	1

# define __issue_syscall(syscall_name)                   \
"       syscall  0x0;\n"

/* Define a macro which expands into the inline wrapper code for a system
   call.  */
# undef INLINE_SYSCALL
# define INLINE_SYSCALL(name, nr, args...)                       \
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

# undef INLINE_SYSCALL_NC
# define INLINE_SYSCALL_NCS(name, nr, args...)                     \
  ({                                                               \
     INTERNAL_SYSCALL_DECL (err);                                  \
     long result_var = INTERNAL_SYSCALL_NCS (name, err, nr, args); \
     if (INTERNAL_SYSCALL_ERROR_P (result_var, err))               \
       {                                                           \
         __set_errno (INTERNAL_SYSCALL_ERRNO (result_var, err));   \
			result_var = -1 ;			   \
       }                                                           \
     result_var;                                                   \
  })
# undef INTERNAL_SYSCALL_DECL
# define INTERNAL_SYSCALL_DECL(err) do { } while (0)

# undef INTERNAL_SYSCALL_ERROR_P
# define INTERNAL_SYSCALL_ERROR_P(val, err) \
       ((unsigned int) (val) >= 0xfffff001u)

# undef INTERNAL_SYSCALL_ERRNO
# define INTERNAL_SYSCALL_ERRNO(val, err) (-(val))

# undef INTERNAL_SYSCALL
# define INTERNAL_SYSCALL(name, err, nr, args...) \
       internal_syscall##nr (__NR_##name, err, args)

# undef INTERNAL_SYSCALL_NCS
# define INTERNAL_SYSCALL_NCS(syscallno, err, nr, args...) \
       internal_syscall##nr (syscallno, err, args)

# define internal_syscall0(name, err, dummy...)                  \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       : __SYSCALL_CLOBBERS);                                    \
       __res;							 \
  })

# define internal_syscall1(name, err, arg1)                      \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

# define internal_syscall2(name, err, arg1, arg2)                \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       , "r" (__arg2)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

# define internal_syscall3(name, err, arg1, arg2, arg3)          \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       , "r" (__arg2)                                            \
       , "r" (__arg3)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

# define internal_syscall4(name, err, arg1, arg2, arg3, arg4)    \
  ({                                                             \
       register long __res  asm ("$r0");                         \
       register long __num asm ("$r15") = (long) (name);   	 \
       register long __arg1 asm ("$r0") = (long) (arg1);         \
       register long __arg2 asm ("$r1") = (long) (arg2);         \
       register long __arg3 asm ("$r2") = (long) (arg3);         \
       register long __arg4 asm ("$r3") = (long) (arg4);         \
       __asm__ volatile (                                        \
       __issue_syscall (name)                                    \
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       , "r" (__arg2)                                            \
       , "r" (__arg3)                                            \
       , "r" (__arg4)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

# define internal_syscall5(name, err, arg1, arg2, arg3, arg4, arg5) \
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
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       , "r" (__arg2)                                            \
       , "r" (__arg3)                                            \
       , "r" (__arg4)                                            \
       , "r" (__arg5)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

# define internal_syscall6(name, err, arg1, arg2, arg3, arg4, arg5, arg6) \
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
       : "=r" (__res)                                            \
       : "r" (__num)                                             \
       , "r" (__arg1)                                            \
       , "r" (__arg2)                                            \
       , "r" (__arg3)                                            \
       , "r" (__arg4)                                            \
       , "r" (__arg5)                                            \
       , "r" (__arg6)                                            \
       : __SYSCALL_CLOBBERS);                                    \
        __res;                                                   \
  })

#define __SYSCALL_CLOBBERS "memory"

#endif /* !__ASSEMBLER__  */

#define PTR_MANGLE(var) (void) (var)
#define PTR_DEMANGLE(var) (void) (var)

#endif /* linux/nds32/sysdep.h  */
