/* Private macros for accessing __jmp_buf contents.  nds32 version.
   Copyright (C) 2006 Free Software Foundation, Inc.
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

#define JB_R6   0
#define JB_R7   1
#define JB_R8   2
#define JB_R9   3
#define JB_R10  4
#define JB_R11  5
#define JB_R12  6
#define JB_R13  7
#define JB_R14  8
#define JB_R16  9
#define JB_R17  10
#define JB_R18  11
#define JB_R19  12
#define JB_FP   13
#define JB_GP   14
#define JB_LP   15
#define JB_SP   16

#ifdef __NDS32_ABI_2FP_PLUS__
#define JB_SIZE 48
#else
#define JB_SIZE 18
#endif
