#include <tcb-offsets.h>

#define SAVE_PID \
  lwi  $r15, [$r25 + PID_OFFSET]; \
  subri $r15, $r15, 0; \
  swi  $r15, [$r25 + PID_OFFSET];

#define RESTORE_PID \
  lwi  $r15, [$r25 + PID_OFFSET]; \
  subri $r15, $r15, 0; \
  swi  $r15, [$r25 + PID_OFFSET];

/*
   Revision History:

    Sep.07.2006     Created by Harry.
                    Initial porting refered from ARM platform.
*/

#include <sysdep.h>
#define _ERRNO_H    1
#include <bits/errno.h>
#include <kernel-features.h>

ENTRY (__vfork)
#ifdef PIC
.pic
#endif	

#ifdef __NR_vfork
    SAVE_PID
    syscall __NR_vfork
    bltz $r0, 2f
1:
    beqz $r0, 4f
    RESTORE_PID
4:
    ret
2:
    sltsi $r1, $r0, -4096
    bnez $r1, 1b;

# ifdef __ASSUME_VFORK_SYSCALL
#  ifdef PIC
	! set r1 as gp
	pushm	$gp,	$lp
	.cfi_adjust_cfa_offset 8
	.cfi_rel_offset gp, 0
	.cfi_rel_offset lp, 4
	mfusr	$r15,	$PC
	sethi	$gp,	hi20(_GLOBAL_OFFSET_TABLE_+4)
	ori	$gp,	$gp,	lo12(_GLOBAL_OFFSET_TABLE_+8)
	add	$gp,	$gp,	$r15

	! r15=C_SYMBOL_NAME(__syscall_error)@PLT
	sethi	$r15,	hi20(C_SYMBOL_NAME(__syscall_error)@PLT)
	ori	$r15,	$r15, lo12(C_SYMBOL_NAME(__syscall_error)@PLT)
	add	$r15,	$r15, $gp
	
	! jump to SYSCALL_ERROR
	jral		$r15
	popm	$gp,	$lp
	.cfi_adjust_cfa_offset -8
	.cfi_restore lp
	.cfi_restore gp
	ret
#  else  /* PIC */
	j C_SYMBOL_NAME(__syscall_error)
#  endif /* PIC */
# else   /* __ASSUME_VFORK_SYSCALL */
    /* Check if vfork syscall is known at all. */
	li $r1, -ENOSYS
	beq	$r0, $r1, 1f

#  ifdef PIC
3:
        ! set r1 as gp
        mfusr	 $r15,   $PC
        sethi   $r1,    hi20(_GLOBAL_OFFSET_TABLE_+4)
        ori     $r1,    $r1,    lo12(_GLOBAL_OFFSET_TABLE_+8)
        add     $r1,    $r1,    $r15

        ! r15=C_SYMBOL_NAME(__syscall_error)@PLT
        sethi $r15, hi20(C_SYMBOL_NAME(__syscall_error)@PLT)
        ori     $r15,   $r15, lo12(C_SYMBOL_NAME(__syscall_error)@PLT)
        add     $r15, $r15, $r1

        ! jump to SYSCALL_ERROR
        jr              $r15
#  else  /* PIC */
        j C_SYMBOL_NAME(__syscall_error)
#  endif /* PIC */
1: 
# endif /* __ASSUME_VFORK_SYSCALL */
#endif /* __NR_vfork */

#ifndef __ASSUME_VFORK_SYSCALL
    /* If we don't have vfork, fork is close enough. */
        SAVE_PID
	syscall __NR_fork
	! bgez $r0, 1f
        beqz $r0, 1f
        RESTORE_PID
        j    1f
	sltsi $r1, $r0, -4096
	bnez	$r1, 1f

# ifdef PIC
        b       3b
# else
        j C_SYMBOL_NAME(__syscall_error)
# endif
1:
    ret

#elif !defined __NR_vfork
# error "__NR_vfork not available and __ASSUME_VFORK_SYSCALL defined"
#endif

PSEUDO_END (__vfork)

libc_hidden_def (__vfork)

weak_alias (__vfork, vfork)
