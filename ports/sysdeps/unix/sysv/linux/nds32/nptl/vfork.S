
#include <tls.h>

/* Save the PID value.  */
#define SAVE_PID \
	lwi  	$r1, [$r25 + PID_OFFSET];/* Get the thread pointer.  */	\
	subri	$r1, $r1, 0x0;		 /* Negate it.  */		\
	bnez	$r1, 1f;		 /* If it was zero... */		\
	sethi	$r1, 0x80000;		 /* use 0x80000000 instead.  */	\
1:	swi	$r1, [$r25 + PID_OFFSET];/* Store the temporary PID.  */

/* Restore the old PID value in the parent.  */
#define RESTORE_PID \
	beqz	$r0, 1f;		/* If we are the parent... */	\
	lwi  	$r1, [$r25 + PID_OFFSET];/* Get the thread pointer.  */	\
	subri	$r1, $r1, 0x0;		/* Re-negate it.  */		\
	sethi	$r2, 0x80000;		/* Load 0x80000000... */	\
	bne	$r1, $r2, 2f;		/* ... compare against it... */	\
	movi	$r1, 0;			/* ... use 0 instead.  */	\
2:	swi	$r1, [$r25 + PID_OFFSET];/* Restore the PID.  */		\
1:

#include <sysdeps/unix/sysv/linux/nds32/vfork.S>
