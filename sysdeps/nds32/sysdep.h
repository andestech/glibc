/* Assembler macros for Andes nds32.
   Copyright (C) 2015-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdeps/generic/sysdep.h>

#ifdef	__ASSEMBLER__

/* Define an entry point visible from C.  */
# ifdef PIC
#  define ENTRY(name)                     \
   .pic					  \
   .align 2;                              \
   .globl C_SYMBOL_NAME(name);            \
   .func  C_SYMBOL_NAME(name);            \
   .type  C_SYMBOL_NAME(name), @function; \
   C_SYMBOL_NAME(name):			  \
   cfi_startproc;
# else
#  define ENTRY(name)                     \
   .align 2;                              \
   .globl C_SYMBOL_NAME(name);            \
   .func  C_SYMBOL_NAME(name);            \
   .type  C_SYMBOL_NAME(name), @function; \
   C_SYMBOL_NAME(name):			  \
   cfi_startproc;
# endif


# undef END
# define END(name)    \
  cfi_endproc;	      \
  .endfunc;           \
  .size C_SYMBOL_NAME(name), .-C_SYMBOL_NAME(name)

/* If compiled for profiling, call `mcount' at the start of each function.  */
# ifdef HAVE_ELF
#  undef NO_UNDERSCORES
#  define NO_UNDERSCORES
# endif

# ifdef NO_UNDERSCORES
#  define syscall_error __syscall_error
# endif


# define GET_GTABLE(reg)					\
	sethi	reg,	hi20(_GLOBAL_OFFSET_TABLE_ - 8);	\
	ori	reg,	reg,	lo12(_GLOBAL_OFFSET_TABLE_ - 4);\
	add5.pc reg;



#endif	/* __ASSEMBLER__ */
  

