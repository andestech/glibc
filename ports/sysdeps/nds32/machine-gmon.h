/* Copyright (C) 2013-2014 Free Software Foundation, Inc.

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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

/* We need a special version of the `mcount' function because it has
   to preserve more registers than your usual function.  */

static void __mcount_internal (u_long frompc, u_long selfpc) __attribute_used__;

#define _MCOUNT_DECL(frompc, selfpc) \
static void __mcount_internal (u_long frompc, u_long selfpc)


#define MCOUNT							\
void _mcount (u_long frompc)		      			\
{								\
  __mcount_internal (frompc, (u_long) RETURN_ADDRESS (0));	\
}

