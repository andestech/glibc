/* Copyright (C) 2003-2013 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <tls.h>
#ifndef __ASSEMBLER__
# include <nptl/pthreadP.h>
#endif

#define PUSHARGS_0
#define PUSHARGS_1	smw.adm $r0, [$sp], $r0
#define PUSHARGS_2	smw.adm $r0, [$sp], $r1
#define PUSHARGS_3	smw.adm $r0, [$sp], $r2
#define PUSHARGS_4	smw.adm $r0, [$sp], $r3
#define PUSHARGS_5	smw.adm $r0, [$sp], $r4
#define PUSHARGS_6	smw.adm $r0, [$sp], $r5

#define POPARGS2_0
#define POPARGS2_1	lmw.bim $r0, [$sp], $r0
#define POPARGS2_2	lmw.bim $r0, [$sp], $r1
#define POPARGS2_3	lmw.bim $r0, [$sp], $r2
#define POPARGS2_4	lmw.bim $r0, [$sp], $r3
#define POPARGS2_5	lmw.bim $r0, [$sp], $r4
#define POPARGS2_6	lmw.bim $r0, [$sp], $r5

#if !defined NOT_IN_libc || defined IS_IN_libpthread || defined IS_IN_librt

/* NOTE: We do mark syscalls with unwind annotations, for the benefit of
   cancellation; but they're really only accurate at the point of the
   syscall.  The ARM unwind directives are not rich enough without adding
   a custom personality function.  */

# undef PSEUDO
# define PSEUDO(name, syscall_name, args)				\
  .align 2;                                                             \
  ENTRY (__##syscall_name##_nocancel);					\
  __do_syscall(syscall_name);                                           \
  PSEUDO_RET;                                                           \
  ret;							                \
  END (__##syscall_name##_nocancel);					\
  ENTRY (name);								\
  smw.adm $r6,[$sp],$r6,0x2;                                            \
  SINGLE_THREAD_P ($r15);                                               \
  bgtz $r15, .Lpseudo_cancel;                                           \
  __do_syscall(syscall_name);                                           \
  j 50f;                                                                \
  .Lpseudo_cancel:							\
	PUSHARGS_##args;	/* save syscall args etc. around CENABLE.  */	\
	CENABLE ($r5);							\
	mov55 $r6, $r0;		/* put mask in safe place.  */    	\
        POPARGS2_##args;                                                \
	__do_syscall(syscall_name);		/* do the call.  */	\
	push $r0;                                                       \
        mov55	$r0, $r6;		/* save syscall return value. */\
	CDISABLE($r5);							\
        pop $r0;                          /* retrieve return value.  */	\
50:\
  lmw.bim $r6,[$sp],$r6, 0x2;                                           \
  PSEUDO_RET;                                                           
# ifndef __ASSEMBLER__
//#  if defined IS_IN_libpthread || !defined NOT_IN_libc
//extern int __local_multiple_threads attribute_hidden;
//#  define SINGLE_THREAD_P __builtin_expect (__local_multiple_threads == 0, 1)
//#  else
/*  There is no __local_multiple_threads for librt */
#  define SINGLE_THREAD_P __builtin_expect (THREAD_GETMEM (THREAD_SELF,       \
				           header.multiple_threads) == 0, 1)
//#  endif
# else
#   define SINGLE_THREAD_P(reg)            \
    addi reg, $r25, MULTIPLE_THREADS_OFFSET; \
    lw   reg, [reg];
#   define SINGLE_THREAD_P_PIC(x) SINGLE_THREAD_P(x)
# endif


# ifdef IS_IN_libpthread
#  define CENABLE(reg)	jmp(reg, __pthread_enable_asynccancel)
#  define CDISABLE(reg) jmp(reg, __pthread_disable_asynccancel)
#  define __local_multiple_threads __pthread_multiple_threads
# elif !defined NOT_IN_libc
#  define CENABLE(reg)	jmp(reg, __libc_enable_asynccancel)
#  define CDISABLE(reg)	jmp(reg, __libc_disable_asynccancel)
#  define __local_multiple_threads __libc_multiple_threads
# elif defined IS_IN_librt
#  define CENABLE(reg)	jmp(reg, __librt_enable_asynccancel)
#  define CDISABLE(reg)	jmp(reg, __librt_disable_asynccancel)
# else
#  error Unsupported library
# endif

#elif !defined __ASSEMBLER__

/* For rtld, et cetera.  */
# define SINGLE_THREAD_P 1
# define NO_CANCELLATION 1

#endif







#ifndef __ASSEMBLER__
# define RTLD_SINGLE_THREAD_P \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF, \
				   header.multiple_threads) == 0, 1)
#endif



#ifdef PIC
# ifdef __NDS32_N1213_43U1H__
# define   PSEUDO_RET \
	 .pic \
   .align 2; \
   bgez $r0, 1f; \
   sltsi $r1, $r0, -4096;    \
   bgtz  $r1, 1f;     \
   mfusr $r15, $PC;  \
   PIC_jmp($r1, SYSCALL_ERROR)\
	 nop; \
   1:
# else
# define PSEUDO_RET \
	 .pic \
   .align 2; \
   bgez $r0, 1f; \
   sltsi $r1, $r0, -4096;    \
   bgtz  $r1, 1f;     \
   mfusr $r15, $PC;  \
   PIC_jmp($r1, SYSCALL_ERROR)\
	 nop; \
   1:
# endif
#else  /* PIC*/
#define PSEUDO_RET \
   .align 2;         \
   bgez  $r0, 1f; \
   sltsi $r1, $r0, -4096; \
   bgtz  $r1, 1f;     \
   j SYSCALL_ERROR; \
   1:
#endif

#ifdef PIC
#define jmp(reg, symble) PIC_jmpr(reg, symble)
/* reg: available register */
#define PIC_jmp(reg, symble) \
   mfusr $r15, $PC;  \
   sethi reg,  hi20(_GLOBAL_OFFSET_TABLE_ + 4);  \
   ori   reg,  reg,  lo12(_GLOBAL_OFFSET_TABLE_ + 8);  \
   add   reg,  $r15, reg;  \
   sethi $r15, hi20(symble@PLT);  \
   ori   $r15, $r15, lo12(symble@PLT);  \
   add   $r15, $r15, reg;  \
   jr    $r15; 

#define PIC_jmpr(reg, symble) \
   mfusr $r15, $PC;  \
   sethi reg,  hi20(_GLOBAL_OFFSET_TABLE_ + 4);  \
   ori   reg,  reg,  lo12(_GLOBAL_OFFSET_TABLE_ + 8);  \
   add   reg,  $r15, reg;  \
   sethi $r15, hi20(symble@PLT);  \
   ori   $r15, $r15, lo12(symble@PLT);  \
   add   $r15, $r15, reg;  \
   jral  $r15; 

#else
#define jmp(reg, symble) jal symble
#endif