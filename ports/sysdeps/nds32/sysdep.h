#include <sysdeps/generic/sysdep.h>

#ifdef	__ASSEMBLER__

/* Define an entry point visible from C.  */
#ifdef PIC
#define ENTRY(name)                      \
  .pic										\
  .align 2;                              \
  .globl C_SYMBOL_NAME(name);            \
  .func  C_SYMBOL_NAME(name);            \
  .type  C_SYMBOL_NAME(name), @function; \
C_SYMBOL_NAME(name):			 \
	CFI_SECTIONS;			 \
	cfi_startproc;
#else
#define ENTRY(name)                      \
  .align 2;                              \
  .globl C_SYMBOL_NAME(name);            \
  .func  C_SYMBOL_NAME(name);            \
  .type  C_SYMBOL_NAME(name), @function; \
C_SYMBOL_NAME(name):			 \
	CFI_SECTIONS;			 \
	cfi_startproc;
#endif

#define CFI_SECTIONS			\
	.cfi_sections .debug_frame

#undef END
#define END(name) \
  cfi_endproc;	      \
  .endfunc;           \
  .size C_SYMBOL_NAME(name), .-C_SYMBOL_NAME(name)

/* If compiled for profiling, call `mcount' at the start of each function.  */
#ifdef HAVE_ELF
#undef NO_UNDERSCORES
#define NO_UNDERSCORES
#endif

#ifdef NO_UNDERSCORES
#define syscall_error __syscall_error
#endif



#endif	/* __ASSEMBLER__ */

#ifndef  __ASSEMBLER__
#include <sysdeps/nds32/bits/atomic.h>
#endif


  

