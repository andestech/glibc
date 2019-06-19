/* profiling support for Andes nds32 version
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

static void __mcount_internal (unsigned long int frompc,
			       unsigned long int selfpc);

#define _MCOUNT_DECL(frompc, selfpc) \
static inline void __mcount_internal (unsigned long int frompc, \
unsigned long int selfpc)


#define MCOUNT								\
void _mcount (unsigned long int frompc)		      			\
{									\
  __mcount_internal (frompc, (unsigned long int) RETURN_ADDRESS (0));	\
}

