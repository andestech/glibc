
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
	fldi $fd31, [$r0 + 248]
	fldi $fd30, [$r0 + 240]
	fldi $fd29, [$r0 + 232]
	fldi $fd28, [$r0 + 224]
	fldi $fd27, [$r0 + 216]
	fldi $fd26, [$r0 + 208]
	fldi $fd25, [$r0 + 200]
	fldi $fd24, [$r0 + 192]
	fldi $fd23, [$r0 + 184]
	fldi $fd22, [$r0 + 176]
	fldi $fd21, [$r0 + 168]
	fldi $fd20, [$r0 + 160]
	fldi $fd19, [$r0 + 152]
	fldi $fd18, [$r0 + 144]
	fldi $fd17, [$r0 + 136]
	fldi $fd16, [$r0 + 128]

.LCFG2:
	flsi $fs31, [$r0 + 124]
	flsi $fs30, [$r0 + 120]
	flsi $fs29, [$r0 + 116]
	flsi $fs28, [$r0 + 112]
	flsi $fs27, [$r0 + 108]
	flsi $fs26, [$r0 + 104]
	flsi $fs25, [$r0 + 100]
	flsi $fs24, [$r0 + 96]
	flsi $fs23, [$r0 + 92]
	flsi $fs22, [$r0 + 88]
	flsi $fs21, [$r0 + 84]
	flsi $fs20, [$r0 + 80]
	flsi $fs19, [$r0 + 76]
	flsi $fs18, [$r0 + 72]
	flsi $fs17, [$r0 + 68]
	flsi $fs16, [$r0 + 64]
.LCFG1:
	flsi $fs15, [$r0 + 60]
	flsi $fs14, [$r0 + 56]
	flsi $fs13, [$r0 + 52]
	flsi $fs12, [$r0 + 48]
	flsi $fs11, [$r0 + 44]
	flsi $fs10, [$r0 + 40]
	flsi $fs9, [$r0 + 36]
	flsi $fs8, [$r0 + 32]
.LCFG0:
	flsi $fs7, [$r0 + 28]
	flsi $fs6, [$r0 + 24]
	flsi $fs5, [$r0 + 20]
	flsi $fs4, [$r0 + 16]
	flsi $fs3, [$r0 + 12]
	flsi $fs2, [$r0 + 8]
	flsi $fs1, [$r0 + 4]
	flsi $fs0, [$r0 + 0]
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
 
