/* Private macros for accessing __jmp_buf contents. Andes nds32 version.
   Copyright (C) 2018 Free Software Foundation, Inc.
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

#define JB_R6   0
#define JB_R7   1
#define JB_R8   2
#define JB_R9   3
#define JB_R10  4
#define JB_R11  5
#define JB_R12  6
#define JB_R13  7
#define JB_R14  8
#define JB_FP   9
#define JB_GP   10
#define JB_LP   11
#define JB_SP   12

#define JB_FRAME_ADDRESS(buf) \
	 ((void *)buf[0].__regs[JB_SP])
