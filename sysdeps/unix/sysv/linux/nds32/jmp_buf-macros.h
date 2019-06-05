/* jump buffer constants.  Andes nds32 version.
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
#ifdef __NDS32_ABI_2FP_PLUS__
# define JMP_BUF_SIZE 320
# define SIGJMP_BUF_SIZE 320
# define JMP_BUF_ALIGN 8
# define SIGJMP_BUF_ALIGN 8
# define MASK_WAS_SAVED_OFFSET 184
# define SAVED_MASK_OFFSET 188
#else
# define JMP_BUF_SIZE 184
# define SIGJMP_BUF_SIZE 184
# define JMP_BUF_ALIGN 8
# define SIGJMP_BUF_ALIGN 8
# define MASK_WAS_SAVED_OFFSET 52
# define SAVED_MASK_OFFSET 56
#endif
