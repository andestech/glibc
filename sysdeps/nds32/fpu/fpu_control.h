/* FPU control word bits.  Andes nds32 version.
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

#ifndef _FPU_CONTROL_H

#include <asm/fp_udfiex_crtl.h>
/*
 * Andes Floating-Point Control Status Register
 * 31-20 -> Reserved
 * 19	 -> RIT (RO)
 * 18	 -> DNIT(RO)
 * 17	 -> IEXT(RO)
 * 16	 -> UDFT(RO)
 * 15	 -> OVFT(RO)
 * 14	 -> DBZT(RO)
 * 13	 -> IVOT(RO)
 * 12	 -> DNZ(RW),Denormalized flush-to-Zero mode.
 * 11	 -> IEXE(RW),IEEE Ineaxct (IEX) exception trapping enable.
 * 10	 -> UDFE(RW),IEEE Underflow (UDF) exception trapping enable.
 * 9	 -> OVFE(RW),IEEE Overflow (OVF) exception trapping enable.
 * 8	 -> DBZE(RW),IEEE Divide by Zero (DBZ) exception trapping enable.
 * 7	 -> IVOE(RW),IEEE Invalid Operation (IVO) exception trapping enable.
 * 6	 -> IEX(RW),IEEE Inexact (IEX) cumulative exception flag.
 * 5	 -> UDF(RW),IEEE Underflow (UDF) cumulative exception flag.
 * 4	 -> OVF(RW),IEEE Overflow (OVF) cumulative exception flag.
 * 3	 -> DBZ(RW),IEEE Divide by Zero (DBZ) cumulative exception flag.
 * 2	 -> IVO(RW),IEEE Invalid Operation (IVO) cumulative exception flag.
 * 1-0	 -> Rounding modes.
 *
 * Rounding modes.
 * 00 - rounding to nearest (RN)
 * 01 - rounding (up) toward plus infinity (RP)
 * 10 - rounding (down)toward minus infinity (RM)
 * 11 - rounding toward zero (RZ)
 *
 */

#define ROUND_MOD_MASK 	0x3

/* Mask for enabling exceptions and for the CAUSE bits.  */
#define ENABLE_MASK	(_FPU_MASK_IEX | _FPU_MASK_OVF | \
			 _FPU_MASK_DBZ | _FPU_MASK_IVO | \
			 _FPU_MASK_UDF)


/* Shift for FE_* flags to get up to the ENABLE bits.  */
#define	ENABLE_SHIFT	5


/* masking of interrupts.  */
#define _FPU_MASK_IEX  	0x0800  /* Invalid operation.  */
#define _FPU_MASK_UDF  	0x0400  /* Underflow.  */
#define _FPU_MASK_OVF  	0x0200  /* Overflow.  */
#define _FPU_MASK_DBZ  	0x0100  /* Division by zero.  */
#define _FPU_MASK_IVO 	0x0080  /* Invalid operation.  */


#define _FPU_ENV_RESERVED	(0xffffe000 | __fpu_control)
#define _FPU_MOD_RESERVED	(0xffffe07c | __fpu_control)
#define _FPU_RESERVED   _FPU_MOD_RESERVED
#define _FPU_DEFAULT    0x00000000

/* Default + exceptions enabled.  */
#define _FPU_IEEE	(_FPU_DEFAULT | 0x00000f80)

/* Type of the control word.  */
typedef unsigned int fpu_control_t;

/* Default control word set at startup.  */
extern fpu_control_t __fpu_control;




#define ENABLE_MASK_SUBOUT (_FPU_MASK_OVF | _FPU_MASK_DBZ | _FPU_MASK_IVO)
#define FPU_MASK_SUBOUT   (_FPU_MASK_IEX | _FPU_MASK_UDF)
#define _FPU_DEFAULT_SUPPORT_SUBNORMAL_OUTPUT	FPU_MASK_SUBOUT
#define __KERNEL_SUPPORT_SUBNOR_OUTPUT \
  (__fpu_control == _FPU_DEFAULT_SUPPORT_SUBNORMAL_OUTPUT)

/* Macros for accessing the hardware control word.  */
/* This is fmrx %0, fpscr.  */
#define _FPU_GETCW(cw) \
  __asm__ __volatile__ ("fmfcsr\t %0\n\t" : "=r" (cw))
/* This is fmxr fpscr, %0.  */
#define _FPU_SETCW(cw) \
  __asm__ __volatile__ ("fmtcsr\t %0\n\t": : "r" (cw))

#endif /* _FPU_CONTROL_H.  */
