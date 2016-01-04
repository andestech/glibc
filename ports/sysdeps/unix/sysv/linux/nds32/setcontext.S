
#include <sysdep.h>
#include <features.h>

#include "ucontext_i.h"

ENTRY(__setcontext)
        move    $r4, $r0

#ifdef NDS32_ABI_2FP_PLUS
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
	fldi $fd31, [$r2 + 248]
	fldi $fd30, [$r2 + 240]
	fldi $fd29, [$r2 + 232]
	fldi $fd28, [$r2 + 224]
	fldi $fd27, [$r2 + 216]
	fldi $fd26, [$r2 + 208]
	fldi $fd25, [$r2 + 200]
	fldi $fd24, [$r2 + 192]
.LCFG2:
	flsi $fs21, [$r2 + 84]
	flsi $fs20, [$r2 + 80]
	flsi $fs19, [$r2 + 76]
	flsi $fs18, [$r2 + 72]
	flsi $fs17, [$r2 + 68]
	flsi $fs16, [$r2 + 64]
.LCFG1:
	flsi $fs15, [$r2 + 60]
	flsi $fs14, [$r2 + 56]
	flsi $fs13, [$r2 + 52]
	flsi $fs12, [$r2 + 48]
	flsi $fs11, [$r2 + 44]
	flsi $fs10, [$r2 + 40]
	flsi $fs9, [$r2 + 36]
	flsi $fs8, [$r2 + 32]
.LCFG0:
	flsi $fs7, [$r2 + 28]
	flsi $fs6, [$r2 + 24]
	/*save fpcsr*/
	lwi $r1, [$r0 + 0x100]
	fmtcsr $r1
#endif /* NDS32_ABI_2FP_PLUS */

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
        push    $r1
        move    $r1, $r0
        lwi     $r0, [$r1 + UCONTEXT_GREGS]
        pop     $r1
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
 
