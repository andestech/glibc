#include <sysdep.h>
#include <features.h>

#include "ucontext_i.h"
/* int swapcontext (ucontext_t *oucp, const ucontext_t *ucp) */

ENTRY(__swapcontext)
	move	$r5, $r1
        swi     $lp, [$r0 + UCONTEXT_PC]
        addi    $r15, $r0, UCONTEXT_GREGS
        xor     $r1, $r1, $r1
	smw.bim	$r1, [$r15], $r1
	smw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	smw.bim	$r16, [$r15], $r25, #0xf
	move    $r4, $r0

	/* sigprocmask (SIG_BLOCK, NULL, &sc->sc_mask).  */
	move    $r0, SIG_BLOCK
	move	$r1, 0
	addi	$r2, $r4, UCONTEXT_SIGMASK
	move	$r3, _NSIG8
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, 1f


#ifdef __NDS32_ABI_2FP_PLUS__
	addi	$r2, $r4, UCONTEXT_FDREGS
/* Process for FPU registers.  */
	fmfcfg	$r20	/* Keep $fpcfg in $r20.  */
	slli	$r20, $r20, #28
	srli	$r20, $r20, #30	/* Set $r20 as $fpcfg.freg.  */

	/* Case switch for $r20 as $fpcfg.freg.  */
	beqz	$r20, .LCFG0		/* Branch if $fpcfg.freg = 0b00.  */
	xori	$r15, $r20, #0b10
	beqz	$r15, .LCFG2		/* Branch if $fpcfg.freg = 0b10.  */
	srli	$r20, $r20, #0b01
	beqz	$r20, .LCFG1		/* Branch if $fpcfg.freg = 0b01.  */
	/* Fall-through if $fpcfg.freg = 0b11.  */
.LCFG3:
	fsdi $fd31, [$r2 + 248]
	fsdi $fd30, [$r2 + 240]
	fsdi $fd29, [$r2 + 232]
	fsdi $fd28, [$r2 + 224]
	fsdi $fd27, [$r2 + 216]
	fsdi $fd26, [$r2 + 208]
	fsdi $fd25, [$r2 + 200]
	fsdi $fd24, [$r2 + 192]
.LCFG2:
	fsdi $fd10, [$r2 + 80]
	fsdi $fd9, [$r2 + 72]
	fsdi $fd8, [$r2 + 64]
.LCFG1:
	fsdi $fd7, [$r2 + 56]
	fsdi $fd6, [$r2 + 48]
	fsdi $fd5, [$r2 + 40]
	fsdi $fd4, [$r2 + 32]
.LCFG0:
	fsdi $fd3, [$r2 + 24]
	/*save fpcsr*/
	fmfcsr $r1
	swi $r1, [$r2 + 0x100]
#endif /* __NDS32_ABI_2FP_PLUS__ */



#ifdef __NDS32_ABI_2FP_PLUS__
	addi	$r0,  $r5, UCONTEXT_FDREGS
	fmfcfg  $r20    /* Keep $fpcfg in $r20.  */
	slli    $r20, $r20, #28
	srli    $r20, $r20, #30 /* Set $r20 as $fpcfg.freg.  */

	fmfcfg  $r20    /* Keep $fpcfg in $r20.  */
	slli    $r20, $r20, #28
	srli    $r20, $r20, #30 /* Set $r20 as $fpcfg.freg.  */

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
.LCFG2:
	fldi $fd10, [$r0 + 80]
	fldi $fd9, [$r0 + 72]
	fldi $fd8, [$r0 + 64]
.LCFG1:
	fldi $fd7, [$r0 + 56]
	fldi $fd6, [$r0 + 48]
	fldi $fd5, [$r0 + 40]
	fldi $fd4, [$r0 + 32]
.LCFG0:
	fldi $fd3, [$r0 + 24]
	/*save fpcsr*/
	lwi $r1, [$r0 + 0x100]
	fmtcsr $r1
#endif /* __NDS32_ABI_2FP_PLUS__ */

	/* sigprocmask (SIG_BLOCK, &sc->sc_mask, NULL).  */
	move    $r0, SIG_SETMASK
	addi	$r1, $r5, UCONTEXT_SIGMASK
	move	$r2, 0
	move	$r3, _NSIG8
	syscall SYS_ify(rt_sigprocmask)
        bnez    $r0, 1f

        move    $r0, $r5
        addi    $r15, $r5, UCONTEXT_GREGS + 4
        addi    $r15, $r15, 20
	lmw.bim	$r1, [$r15], $r14
        addi    $r15, $r15, 4
	lmw.bim	$r16, [$r15], $r25, #0xf
        lwi     $r15, [$r0 + UCONTEXT_PC]
        lwi     $r0, [$r0 + UCONTEXT_GREGS]
        jr      $r15

1:
	move $r0, -1
	ret
END(__swapcontext)

weak_alias (__swapcontext, swapcontext)
