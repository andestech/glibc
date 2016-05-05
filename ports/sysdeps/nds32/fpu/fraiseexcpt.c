/* Raise given exceptions.
   Copyright (C) 2004-2013 Free Software Foundation, Inc.
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

#include <fpu_control.h>
#include <fenv.h>
#include <float.h>
#include <fenv_libc.h>


int
feraiseexcept (int excepts)
{
	float temp1 = 0.0, temp2 = 1.0;
#if __NDS32_ABI_2FP_PLUS__
	if (FE_INVALID & excepts)
	{
		asm volatile(
			"fmtsr\t %0, $fs0\n\t"
			"fdivs\t $fs0, $fs0, $fs0\n\t"
			:
			:"r"(temp1)
			:"$fs0"
			);
	}
	if (FE_DIVBYZERO & excepts)
	{
		asm volatile(
			"fmtsr\t %0, $fs0\n\t"
			"fmtsr\t %1, $fs1\n\t"
			"fdivs\t $fs0, $fs1, $fs0\n\t"
			:
			:"r"(temp1),"r"(temp2)
			:"$fs0"
			);
	}
	if (FE_OVERFLOW & excepts)
	{
	/* There's no way to raise overflow without also raising inexact.
	*/
		unsigned int fpcsr;
		temp1 = FLT_MAX;
		asm volatile(
			"fmfcsr\t %0\n\t"
			"fmtsr\t %1, $fs0\n\t"
			"fadds\t $fs0, $fs0, $fs0\n\t"
			"ori\t %0,%0,0x10\n\t"
			"fmtcsr\t %0\n\t"
			:"=&r"(fpcsr)
			:"r"(temp1)
			:"$fs0"
			);
	}
	if (FE_UNDERFLOW & excepts)
	{
	/* There's no way to raise overflow without also raising inexact.
	*/
		temp1 = FLT_MIN;
		temp2 = 2.0;
		asm volatile(
			"fmtsr\t %0, $fs0\n\t"
			"fmtsr\t %1, $fs1\n\t"
			"fdivs\t $fs1, $fs0, $fs1\n\t"
			:
			:"r"(temp1),"r"(temp2)
			:"$fs0","$fs1"
			);
	}
	if (FE_INEXACT & excepts)
	{
		temp1 = 3.0;
		asm volatile(
			"fmtsr\t %0, $fs1\n\t"
			"fmtsr\t %1, $fs0\n\t"
			"fdivs\t $fs1, $fs0, $fs1\n\t"
			:
			:"r"(temp1),"r"(temp2)
			:"$fs0","$fs1"
			);
	}

	return 0;

#endif
	/* Unsupported, so fail unless nothing needs to be done.  */
	return (excepts != 0);
}

libm_hidden_def (feraiseexcept)
