/* Copyright (C) 1998, 1999, 2001 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* System V/ARM ABI compliant context switching support.  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <signal.h>
#include <sys/procfs.h>

typedef int greg_t;

/* Number of general registers.  */
/* REG_R26 and REG_R27 are reserved for kernel */
#define NGREG	(32-2)

/* Container for all general registers.  */
typedef greg_t gregset_t[NGREG];

/* Number of each register is the `gregset_t' array.  */
enum
{
  REG_R0 = 0,
# define REG_R0     REG_R0
  REG_R1 = 1,
# define REG_R1     REG_R1
  REG_R2 = 2,
# define REG_R2     REG_R2
  REG_R3 = 3,
# define REG_R3     REG_R3
  REG_R4 = 4,
# define REG_R4     REG_R4
  REG_R5 = 5,
# define REG_R5     REG_R5
  REG_R6 = 6,
# define REG_R6     REG_R6
  REG_R7 = 7,
# define REG_R7     REG_R7
  REG_R8 = 8,
# define REG_R8     REG_R8
  REG_R9 = 9,
# define REG_R9     REG_R9
  REG_R10 = 10,
# define REG_R10     REG_R10
  REG_R11 = 11,
# define REG_R11     REG_R11
  REG_R12 = 12,
# define REG_R12     REG_R12
  REG_R13 = 13,
# define REG_R13     REG_R13
  REG_R14 = 14,
# define REG_R14     REG_R14
  REG_R15 = 15,
# define REG_R15     REG_R15
  REG_R16 = 16,
# define REG_R16     REG_R16
  REG_R17 = 17,
# define REG_R17     REG_R17
  REG_R18 = 18,
# define REG_R18     REG_R18
  REG_R19 = 19,
# define REG_R19     REG_R19
  REG_R20 = 20,
# define REG_R20     REG_R20
  REG_R21 = 21,
# define REG_R21     REG_R21
  REG_R22 = 22,
# define REG_R22     REG_R22
  REG_R23 = 23,
# define REG_R23     REG_R23
  REG_R24 = 24,
# define REG_R24     REG_R24
  REG_R25 = 25,
# define REG_R25     REG_R25
/*
  These two are reserved for kernel only
  REG_R26 = 26,
  #define REG_R26     REG_R26
  REG_R27 = 27,
  #define REG_R27     REG_R27
*/
  REG_R28 = 26,
# define REG_R28     REG_R28
  REG_R29 = 27,
# define REG_R29     REG_R29
  REG_R30 = 28,
# define REG_R30     REG_R30
  REG_R31 = 29,
# define REG_R31     REG_R31
  REG_FP = REG_R28,
# define REG_FP      REG_FP
  REG_GP = REG_R29,
# define REG_GP      REG_GP
  REG_LP = REG_R30,
# define REG_LP      REG_LP
  REG_SP = REG_R31
# define REG_SP      REG_SP
};

/* Structure to describe FPU registers.
typedef elf_fpregset_t	fpregset_t; */

struct fpu_struct {
    unsigned long fs_regs[32];
    unsigned long long fd_regs[16];
    unsigned long fpcsr;
};

struct audio_struct {
    unsigned long auregs[32];
};

struct zol_struct {
        unsigned long nds32_lc; /* $LC */
        unsigned long nds32_le; /* $LE */
        unsigned long nds32_lb; /* $LB */
};

/* Context to describe whole processor state.  */
typedef struct
  {
    unsigned long trap_no;
    unsigned long error_code;
    unsigned long oldmask;
    gregset_t gregs;
    unsigned long nds32_ipc;
    unsigned long fault_address;
    /*
    cole Jan 4th 2011
    Where is come from? It doesn't appear in kernel sigcontext
    fpregset_t fpregs; */

    /* These two structs cause mcontext_t to align 8 */
    struct fpu_struct fpu; 
    struct audio_struct audio; 
    struct zol_struct zol;
  } mcontext_t;

/* Userlevel context.  */
typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    /* FIXME: the __sigset_t seems not match the one in kernel */
    __sigset_t uc_sigmask;
  } ucontext_t;

#endif /* sys/ucontext.h */
