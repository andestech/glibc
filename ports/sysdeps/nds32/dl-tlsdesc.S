/* Thread-local storage handling in the ELF dynamic linker.  NDS32 version.
   Copyright (C) 2006-2013 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <tls.h>
#include "tlsdesc.h"

	.text
	.hidden _dl_tlsdesc_return
	.global	_dl_tlsdesc_return
	.type	_dl_tlsdesc_return,#function
	.align 2
_dl_tlsdesc_return:
	lwi	$r0	,[$r0]
	ret

     /* This function is used for undefined weak TLS symbols, for
	which the base address (i.e., disregarding any addend) should
	resolve to NULL.

	$r0 points to the TLS descriptor, such that $r0+4 points to
	_dl_tlsdesc_undefweak itself, and $r0 holds the addend.
	We return the addend minus the TP, such that, when the caller
	adds TP, it gets the addend back.  If that's zero, as usual,
	that's most likely a NULL pointer.  */

	.hidden _dl_tlsdesc_undefweak
	.global	_dl_tlsdesc_undefweak
	.type	_dl_tlsdesc_undefweak,#function
	.align 2
_dl_tlsdesc_undefweak:
	sub	$r0,	$r25,	$r0
	ret

#ifdef SHARED
	.hidden _dl_tlsdesc_dynamic
	.global	_dl_tlsdesc_dynamic
	.type	_dl_tlsdesc_dynamic,#function
	.pic
/*
	The assembly code that follows is a rendition of the following
	C code, hand-optimized a little bit.

ptrdiff_t
_dl_tlsdesc_dynamic(struct tlsdesc *tdp)
{
       struct tlsdesc_dynamic_arg *td = tdp->argument.pointer;
       dtv_t *dtv = (dtv_t *)THREAD_DTV();
       if (__builtin_expect (td->gen_count <= dtv[0].counter
                             && dtv[td->tlsinfo.ti_module].pointer.val
                                != TLS_DTV_UNALLOCATED,
                             1))
               return dtv[td->tlsinfo.ti_module].pointer.val +
                       td->tlsinfo.ti_offset - __builtin_thread_pointer();

       return __tls_get_addr (&td->tlsinfo) - __builtin_thread_pointer();
}
*/
	.align 2

_dl_tlsdesc_dynamic:
	push	$lp
	lwi	$r1,	[$r0 + #TLSDESC_GEN_COUNT]  	/* $r0=td $r1=td->gen_count*/
	addi	$r2,	$r25,	 #DTV_OFFSET	    	/* $r2=&dtv[1]*/
	lwi	$r3,	[$r2]
	sub	$r1,	$r1,	$r3
	bgtz	$r1,	2f
	lwi	$r3,	[$r0 + #TLSDESC_MODID]	    	/* r3=module id */
	slli	$r3,	$r3,	#3
	lw	$r3,	[$r2 +	$r3]		   	/* r3=&dtv[module id]*/
	movi	$r1,	#-1
	beq	$r3,	$r1,	2f
	lwi	$r1,	[$r0 + #TLSDESC_MODOFF]
	add	$r0,	$r3,	$r1
1:	
	sub 	$r0,	$r0,	$r25	/*$r0= symbol address - thread pointer*/
	pop	$lp
	ret
2:
	mfusr 	$r15, $PC;
	sethi 	$gp,  hi20(_GLOBAL_OFFSET_TABLE_ + 4);	
	ori   	$gp,  $gp,  lo12(_GLOBAL_OFFSET_TABLE_ + 8);
	add   	$gp,  $r15, $gp;
	sethi 	$r15, hi20(__tls_get_addr@PLT);
	ori	$r15, $r15, lo12(__tls_get_addr@PLT);
	add	$r15, $r15, $gp
	jral 	$r15
	pop	$gp
	j	1b
#endif
