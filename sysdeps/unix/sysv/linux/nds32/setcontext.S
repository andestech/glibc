
#include <sysdep.h>
#include <features.h>

#include "ucontext_i.h"

ENTRY(__setcontext)
        move    $r4, $r0

#ifdef NDS32_ABI_2FP
	lwi	$r0, [$r4 + UCONTEXT_FSREGS]
	lwi.bi	$r20, [$r0], #4	/* Load $fpcfg.freg to $r20.  */

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz    $r20, .LCFG0         /* Branch if $fpcfg.freg = 0b00.  */
	xori    $r15, $r20, #0b10
	beqz    $r15, .LCFG2         /* Branch if $fpcfg.freg = 0b10.  */
	srli    $r20, $r20, #0b01
	beqz    $r20, .LCFG1         /* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
	fldi.bi	$fd31, [$r0], #8
	fldi.bi	$fd29, [$r0], #8
	fldi.bi	$fd27, [$r0], #8
	fldi.bi	$fd25, [$r0], #8
	fldi.bi	$fd23, [$r0], #8
	fldi.bi	$fd21, [$r0], #8
	fldi.bi	$fd19, [$r0], #8
	fldi.bi	$fd17, [$r0], #8
.LCFG2:
	fldi.bi	$fd15, [$r0], #8
	fldi.bi	$fd13, [$r0], #8
	fldi.bi	$fd11, [$r0], #8
	fldi.bi	$fd9, [$r0], #8
.LCFG1:
	fldi.bi	$fd7, [$r0], #8
	fldi.bi	$fd5, [$r0], #8
.LCFG0:
	fldi.bi	$fd3, [$r0], #8
#endif /* NDS32_ABI_2FP */

	/* sigprocmask (SIG_BLOCK, &sc->sc_mask, NULL).  */
	move    $r0, SIG_SETMASK
	lwi	$r1, [$r4 + UCONTEXT_SIGMASK]
	move	$r2, 0
	move	$r3, _NSIG8
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, 1f

        move    $r0, $r4
        addi    $r15, $r4, UCONTEXT_GREGS + 4
	lmw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0, [$r0 + UCONTEXT_GREGS]
        jr      $r15
1:
	move $r0, -1
	ret
END(__setcontext)

weak_alias (__setcontext, setcontext)

ENTRY (__startcontext)
	beqz $r6, 1f
        move $r0, $r6
	jal __setcontext
1:	
	move $r0, 0
	j HIDDEN_JUMPTARGET(exit)
END (__startcontext)

.hidden __startcontext
 