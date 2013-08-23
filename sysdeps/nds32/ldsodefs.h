/* Run-time dynamic linker data structures for loaded ELF shared objects.
   Copyright (C) 1995-2012 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef	_NDS32_LDSODEFS_H
#define	_NDS32_LDSODEFS_H	1

#include <elf.h>

struct La_nds32_regs;
struct La_nds32_retval;

#define ARCH_PLTENTER_MEMBERS						\
    Elf32_Addr (*nds32_gnu_pltenter) (Elf32_Sym *, unsigned int, uintptr_t *, \
				    uintptr_t *, struct La_nds32_regs *,	\
				    unsigned int *, const char *name,	\
				    long int *framesizep)

#define ARCH_PLTEXIT_MEMBERS						\
    unsigned int (*nds32_gnu_pltexit) (Elf32_Sym *, unsigned int, uintptr_t *, \
				     uintptr_t *, const struct La_nds32_regs *, \
				     struct La_nds32_retval *, const char *)

#include_next <ldsodefs.h>

#endif
