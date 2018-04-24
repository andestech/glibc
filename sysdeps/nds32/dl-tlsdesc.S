/* Thread-local storage handling in the ELF dynamic linker, Andes nds32 version.
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
	cfi_startproc
	.align 2
_dl_tlsdesc_return:
	lwi	$r0,	[$r0	+ 4]
	add	$r0,	$r0,	$r25
	ret
	cfi_endproc
	.size   _dl_tlsdesc_return, .-_dl_tlsdesc_return

	.hidden _dl_tlsdesc_undefweak
	.global	_dl_tlsdesc_undefweak
	.type	_dl_tlsdesc_undefweak,#function
	cfi_startproc
	.align 2
_dl_tlsdesc_undefweak:
	ret
	cfi_endproc
	.size   _dl_tlsdesc_undefweak, .-_dl_tlsdesc_undefweak

#ifdef SHARED
	/* Handler for dynamic TLS symbols.
	   Prototype:
	   _dl_tlsdesc_dynamic (tlsdesc *) ;

	  ptrdiff_t
	  _dl_tlsdesc_dynamic(struct tlsdesc *tdp)
	  {
            struct tlsdesc_dynamic_arg *td = tdp->argument.pointer;
            dtv_t *dtv = (dtv_t *)THREAD_DTV();
            if (__builtin_expect (td->gen_count <= dtv[0].counter
               && (dtv[td->tlsinfo.ti_module].pointer.val
                  != TLS_DTV_UNALLOCATED),
               1))
               return dtv[td->tlsinfo.ti_module].pointer.val +
                       td->tlsinfo.ti_offset;

             return __tls_get_addr (&td->tlsinfo);
           }
	*/
	.hidden _dl_tlsdesc_dynamic
	.global	_dl_tlsdesc_dynamic
	.type	_dl_tlsdesc_dynamic,#function
	cfi_startproc
	.pic
	.align 2

_dl_tlsdesc_dynamic:
	lwi	$r0,	[$r0 + 4]                  /* $r0 = td.  */
	lwi	$r1,	[$r0 + #TLSDESC_GEN_COUNT] /* $r1 = td->gen_count.  */
	lwi	$r2,	[$r25 +	#DTV_OFFSET]	   /* $r2 = &dtv[0].  */
	lwi	$r3,	[$r2]                      /* $r3 = module id.  */
	sub	$r1,	$r1,	$r3
	bgtz	$r1,	2f
	lwi	$r3,	[$r0 + #TLSDESC_MODID]
	slli	$r3,	$r3,	#3  /* $r3 = module offset = ID * 8.  */
	lw	$r3,	[$r2 + $r3] /* $r3 = &dtc[ID] = &dtv[0]+ module offset.  */
	movi	$r1,	#TLS_DTV_UNALLOCATED
	beq	$r3,	$r1,	2f
	lwi	$r1,	[$r0 + #TLSDESC_MODOFF]
	add	$r0,	$r3,	$r1
1:	
	ret
2:
	smw.adm $sp,[$sp],$sp,#0x6
	.cfi_adjust_cfa_offset 8
	.cfi_rel_offset gp, 0
	.cfi_rel_offset lp, 4
	GET_GTABLE ($gp)
	la	$r15, HIDDEN_JUMPTARGET (__tls_get_addr@PLT)
	jral 	$r15
	lmw.bim $sp,[$sp],$sp,#0x6
	.cfi_adjust_cfa_offset -8
	.cfi_restore gp
	.cfi_restore lp
	j	1b
	cfi_endproc
	.size   _dl_tlsdesc_dynamic, .-_dl_tlsdesc_dynamic
#endif /* SHARED */
