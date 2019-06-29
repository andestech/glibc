/* Assembler macros.  Andes nds32.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
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
# ifdef __PIC__
#  define ENTRY(name)	   \
   .pic			   \
   .align 2;               \
   .globl name;            \
   .func  name;            \
   .type  name, @function; \
   name:		   \
   cfi_startproc;
# else
#  define ENTRY(name)		\
   .align 2;			\
   .globl name;			\
   .func  name;			\
   .type  name, @function;	\
   name:			\
   cfi_startproc;
# endif /* !__PIC__ */

# undef END
# define END(name)    \
  cfi_endproc;	      \
  .endfunc;           \
  .size name, .-name

# define GET_GTABLE(reg)				\
    sethi   reg, hi20(_GLOBAL_OFFSET_TABLE_ - 8);	\
    ori	    reg, reg, lo12(_GLOBAL_OFFSET_TABLE_ - 4);  \
    add5.pc reg;



#endif	/* __ASSEMBLER__.  */
  

