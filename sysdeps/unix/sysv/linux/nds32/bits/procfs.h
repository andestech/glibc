/* Types for registers for sys/procfs.h.  Andes nds32 version.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
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

#ifndef _SYS_PROCFS_H
# error "Never include <bits/procfs.h> directly; use <sys/procfs.h> instead."
#endif

#include <asm/ptrace.h>

/* Type for a general-purpose register.  */
typedef unsigned long int elf_greg_t;
/* Type for a floating-point registers.  */
typedef long long int  elf_fpreg_t;

#define ELF_NGREG       (sizeof(struct user_pt_regs)/sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

/* Register set for the floating-point registers.  */
#define ELF_NFPREG 	33
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];
