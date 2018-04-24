/* Macros to support TLS testing in times of missing compiler support.
   Andes nds32 version.
   Copyright (C) 2017-2018 Free Software Foundation, Inc.

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

#define TLS_LE(x) 						\
({	int *__l;						\
	asm("sethi	%0, hi20("#x"@TPOFF)\n\t"		\
            "ori	%0, %0, lo12("#x"@TPOFF)\n\t"		\
            "add	%0, %0, $r25\n\t"			\
	    : "=r" (__l));					\
	__l;})

#ifdef PIC
# define TLS_IE(x)						\
({	int *__l;						\
	asm(".relax_hint begin\n\t"				\
            "sethi	%0, hi20("#x"@GOTTPOFF)\n\t"		\
            ".relax_hint\n\t"					\
            "ori	%0, %0, lo12("#x"@GOTTPOFF)\n\t"	\
            ".relax_hint end\n\t"				\
            "lw	%0, [%0 + $gp]\n\t"				\
            "add	%0, %0, $r25\n\t"			\
            : "=r" (__l));					\
	__l;})
#else
# define TLS_IE(x) 						\
({	int *__l;						\
	asm(".relax_hint begin\n\t"				\
            "sethi 	%0, hi20("#x"@GOTTPOFF)\n\t"		\
            ".relax_hint end\n\t"				\
            "lwi 	%0, [%0 + lo12("#x"@GOTTPOFF)]\n\t"	\
            "add	%0, %0, $r25\n\t"			\
            : "=r" (__l));					\
	__l;})
#endif

#define TLS_LD(x) TLS_GD(x)

#define TLS_GD(x)						\
({	int *__l;	                                        \
	asm("smw.adm $r1,[$sp],$r5,#0\n\t"			\
            "smw.adm $r16,[$sp],$r24,#0\n\t"			\
            ".relax_hint begin\n\t"				\
            "sethi	$r0, hi20("#x"@TLSDESC)\n\t"		\
            ".relax_hint\n\t"					\
            "ori	$r0, $r0, lo12("#x"@TLSDESC)\n\t"	\
            ".relax_hint\n\t"					\
            "lw 	$r15, [$r0 + $gp]\n\t"			\
            ".relax_hint\n\t"					\
            "add 	$r0, $r0, $gp\n\t"			\
            ".relax_hint end\n\t"				\
            "jral 	$r15\n\t"	        		\
            "lmw.bim $r16,[$sp],$r24,#0\n\t"                	\
            "lmw.bim $r1,[$sp],$r5,#0\n\t"                  	\
            "move	%0, $r0\n\t"				\
            : "=r" (__l)					\
            :							\
            : "$r0", "$r15");					\
	__l;})
