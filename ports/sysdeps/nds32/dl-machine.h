/* Machine-dependent ELF dynamic relocation inline functions.  NDS32 version.
   Copyright (C) 1995,1996,1997,1999,2000,2001 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "NDS32"

#include <string.h>
#include <bits/linkmap.h>
#include <fcntl.h>
#include <ldsodefs.h>
#include <nds32-elf.h>


#define BUFLEN 1024

static int feature_set = 0 ;
static nds32_elfinfo_t target ;

#if defined(NDS32_ABI_2)
const static int _default_ABI_ = E_NDS_ABI_AABI ;
#elif defined(NDS32_ABI_2FP)
const static int _default_ABI_ = E_NDS_ABI_V2FP ;
#else
const static int _default_ABI_ = E_NDS_ABI_V1 ;
#endif

// the feature string sequence here shall match the position of EF_NDS_INST - 8
// and the contents shall match the info in /proc/cpuinfo
// see nds32.h and /proc/cpuinfo for detail
static const char *cpu_feature_token[] = {
  "mfusr_pc",
  "perf1",
  "perf2",
  "fpu",
  "audio",
  "div",
  "16b",
  "string",
  "reduce_regs",
  "video",
  "encrypt",
  "fpu-dp",
  "mac",
  "l2c"
} ;

typedef enum reg_index
{
REG_CPU_VER,
REG_MMU_CFG,
REG_MSC_CFG,
REG_FUCOP_EXIST,
REG_FPCFG,
REG_LAST                // this is the last one
} REG_INDEX ;

typedef struct
{
  const char * fname;
  unsigned int set;
  unsigned int val;
} REG_INFO;

// For each element the sequence must match REG_INDEX
static REG_INFO reg_info[] = {
  { "/proc/cpu/cpu_ver", 0, 0 },
  { "/proc/cpu/mmu_cfg", 0, 0 },
  { "/proc/cpu/msc_cfg", 0, 0 },
  { "/proc/cpu/fucop_exist", 0, 0 },
  /* Note: OS don't have this node !!
     Use the struct to keep value only */
  { "/proc/cpu/fpcfg", 0, 0 },
} ;

static unsigned int read_fpu_fpcfg(void)
{
/* We will check predifined marco to avoid compliation error
   if non-FPU toolchain used */
#if defined(__NDS32_EXT_FPU_SP__) || defined(__NDS32_EXT_FPU_DP__)
  unsigned int fpcfg = 0;
  asm volatile ("fmfcfg\t%0\n\t":"=&r" (fpcfg));
  return fpcfg;
#else /* defined(__NDS32_EXT_FPU_SP__) || defined(__NDS32_EXT_FPU_DP__)  */
/* The checking logic accept 0 if machine without FPU */
  return 0;
#endif /* defined(__NDS32_EXT_FPU_SP__) || defined(__NDS32_EXT_FPU_DP__)  */
}

static int
get_cpu_reg_info(REG_INDEX reg_num)
{
  int fd;
  char buf[8000];
  int i;
  int len;
  char * p1;
  unsigned long cpu_feature = E_NDS_ABI_V1 | E_NDS32_ELF_VER_1_3;
  int feature_set = 0;
  REG_INDEX ndx;

  // set all registers' info at once
  for (ndx = REG_CPU_VER; ndx < REG_LAST; ndx++)
    {
      reg_info[ndx].set = 1;
    }

  /* cpu feature has not been set; get CPU info from /proc */
  fd = __open ("/proc/cpuinfo", O_RDONLY);
  if (fd != -1)
    {
      // has CPU info, read info and set feature flags
      len = __libc_read (fd, buf, 8000);
      __close (fd);
      buf[len] = 0;
#if 0
      _dl_error_printf( buf);
#endif

      // find feature string
      p1 = strstr (buf, "Features");

      // set feature flags
      if (p1)
        {
          feature_set = 1;

          // terminate feature string
          strtok (p1, "\r\n");

          // parse it
          p1 = strtok (p1, " :");
          while (p1)
            {
#if 0
              _dl_error_printf( p1);
              _dl_error_printf( "\r\n");
#endif
              for (i = 0; i < sizeof(cpu_feature_token) / sizeof(char *); i++)
                {
                  if (!strcmp (cpu_feature_token[i], p1))
                    {
                      // feature found; set the flag, starting from bit 8
                      cpu_feature |= 1 << (i + 8);
                      break;
                    }
                }

              p1 = strtok (NULL, " ");
            }
        }
    }

  if (!feature_set)
    {
      // no CPU info in /proc; just turn on all features
      cpu_feature |= E_NDS_ARCH_STAR_V1_0 | E_NDS32_HAS_MFUSR_PC_INST
          | E_NDS32_HAS_EXT_INST | E_NDS32_HAS_EXT2_INST | E_NDS32_HAS_FPU_INST
          | E_NDS32_HAS_AUDIO_INST | E_NDS32_HAS_DIV_INST
          | E_NDS32_HAS_FPU_DP_INST | E_NDS32_HAS_16BIT_INST
          | E_NDS32_HAS_STRING_INST | E_NDS32_HAS_NO_MAC_INST
          | E_NDS32_HAS_VIDEO_INST | E_NDS32_HAS_L2C_INST
          | E_NDS32_HAS_ENCRIPT_INST;
    }

  if (!(cpu_feature & E_NDS32_HAS_MFUSR_PC_INST))
    {
      // no mfusr_pc, so set it to hard core
      reg_info[REG_CPU_VER].val |= 0x0c010000;
    }

  if (cpu_feature & E_NDS32_HAS_EXT_INST)
    {
      reg_info[REG_CPU_VER].val |= 1 << 0;
    }

  if (cpu_feature & E_NDS32_HAS_16BIT_INST)
    {
      reg_info[REG_CPU_VER].val |= 1 << 1;
    }

  if (cpu_feature & E_NDS32_HAS_EXT2_INST)
    {
      reg_info[REG_CPU_VER].val |= 1 << 2;
    }

  if (cpu_feature & (E_NDS32_HAS_FPU_INST | E_NDS32_HAS_FPU_DP_INST))
    {
      reg_info[REG_CPU_VER].val |= 1 << 3;
      reg_info[REG_FUCOP_EXIST].val |= 0x80000000;
    }

  if (cpu_feature & E_NDS32_HAS_STRING_INST)
    {
      reg_info[REG_CPU_VER].val |= 1 << 4;
    }

  if (cpu_feature & E_NDS32_HAS_DIV_INST)
    {
      reg_info[REG_MSC_CFG].val |= 1 << 5;
    }

  if (cpu_feature & E_NDS32_HAS_NO_MAC_INST)
    {
      reg_info[REG_MSC_CFG].val |= 1 << 6;
    }

  if (cpu_feature & E_NDS32_HAS_AUDIO_INST)
    {
      reg_info[REG_MSC_CFG].val |= 1 << 7;
    }

  if (cpu_feature & E_NDS32_HAS_L2C_INST)
    {
      reg_info[REG_MSC_CFG].val |= 1 << 9;
    }

  if (cpu_feature & E_NDS32_HAS_REDUCED_REGS)
    {
      reg_info[REG_MSC_CFG].val |= 1 << 10;
    }

  if (cpu_feature & E_NDS32_HAS_FPU_INST)
    {
      reg_info[REG_FPCFG].val |= 1 << 0;
    }

  if (cpu_feature & E_NDS32_HAS_FPU_DP_INST)
    {
      reg_info[REG_FPCFG].val |= 1 << 1;
    }

  return reg_info[reg_num].val;
}

static unsigned int
reg_read(unsigned int index)
{
  char * p;
  int fd;
  char buf[16];
  int len;
  unsigned int val = 0;
  REG_INDEX reg_num;

  switch (index)
    {
  case 0:
    // CPU_VER
    reg_num = REG_CPU_VER;
    break;

  case 24:
    // MMU_CFG
    reg_num = REG_MMU_CFG;
    break;

  case 32:
    // MSC_CFG
    reg_num = REG_MSC_CFG;
    break;

  case 40:
    // FUCOP_EXIST
    reg_num = REG_FUCOP_EXIST;
    break;

  case 0x400:
    // FPCFG
    /* Read FPU CONFIG use fmfcfg to get value */
    return read_fpu_fpcfg();
    break;

  default:
    return 0;
    break;
    }

  if (reg_info[reg_num].set)
    {
      // value has been set, just return the value
      return reg_info[reg_num].val;
    }

  // has not been set, read it from file
  fd = __open (reg_info[reg_num].fname, O_RDONLY);
  if (fd == -1)
    {
      // can't open register info file, read CPU info file
      return get_cpu_reg_info (reg_num);
    }

  len = __libc_read (fd, buf, 16);
  __close (fd);
  buf[len] = 0;
  //      _dl_error_printf(buf);

  p = strstr (buf, "0x");
  if (p)
    {
      p += 2;
      while (*p)
        {
          if (*p >= '0' && *p <= '9')
            {
              val = val * 16 + *p - '0';
            }
          else if (*p >= 'a' && *p <= 'f')
            {
              val = val * 16 + *p - 'a' + 10;
            }
          else if (*p >= 'A' && *p <= 'F')
            {
              val = val * 16 + *p - 'A' + 10;
            }
          else
            {
              break;
            }
          p++;
        }
    }

  // save it for next time
  reg_info[reg_num].val = val;
  reg_info[reg_num].set = 1;

  return val;
}

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int __attribute__ ((unused))
elf_machine_matches_host(const Elf32_Ehdr *ehdr)
{
  int fd;
  char buf[8000];
  int len;

  /* check ABI first */
  if (_default_ABI_ != (ehdr->e_flags & EF_NDS_ABI))
    {
      _dl_error_printf ("ld.so: Shared Object ABI mismatch");
      return 0;
    }

  /* does uses turn on/off ELF/CPU checking */
  fd = __open ("/proc/cpu/elf_core_checking", O_RDONLY);
  if (fd == -1)
    {
      // ET: 121109 per kelly's request
      // no checking, just return OK
      return 1;
      // fd = __open ("/elf_core_checking", O_RDONLY) ;
    }

  if (fd != -1)
    {
      // file open OK, check contents
      len = __libc_read (fd, buf, 8000);
      __close (fd);
      buf[len] = 0;
      if (strchr (buf, '0'))
        {
          // no checking, just return OK
          return 1;
        }
    }

  // no need to check endian since it was checked in  open_verify()
  unsigned int status;
  unsigned int err = elf_check (ehdr, reg_read, buf, 8000, &status);

  if (err)
    {
  //    nds32_elf_get_errstr (buf, 8000, err);
      _dl_error_printf (buf);
      return 0;
    }

  return 1;
}


#if defined PI_STATIC_AND_HIDDEN \
    && defined HAVE_VISIBILITY_ATTRIBUTE && defined HAVE_HIDDEN \
    && !defined HAVE_BROKEN_VISIBILITY_ATTRIBUTE

/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT, a special entry that is never relocated.  */
static inline Elf32_Addr __attribute__ ((unused, const))
elf_machine_dynamic (void)
{
  /* This produces a GOTOFF reloc that resolves to some offset at link time, so in
     fact just loads from the GOT register directly.  By doing it without
     an asm we can let the compiler choose any register.  */
//   extern Elf32_Addr __attribute__((visibility("hidden"))) GLOBAL_OFFSET_TABLE_[];
//	return GLOBAL_OFFSET_TABLE_[0]	;
  extern const Elf32_Addr _GLOBAL_OFFSET_TABLE_[] attribute_hidden;
  return _GLOBAL_OFFSET_TABLE_[0];
}

/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr __attribute__ ((unused))
elf_machine_load_address (void)
{
  /* Compute the difference between the runtime address of _DYNAMIC as seen
     by a GOTOFF reference, and the link-time address found in the special
     unrelocated first GOT entry.  */
  extern Elf32_Dyn bygotoff[] asm ("_DYNAMIC") attribute_hidden;
  return (Elf32_Addr) &bygotoff - elf_machine_dynamic ();
}

#else  /* Without .hidden support, we can't compile the code above.  */

/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static inline Elf32_Addr __attribute__ ((unused))
elf_machine_dynamic (void)
{
	Elf32_Addr link_addr ;
	
	asm ( "l.w %0, _GLOBAL_OFFSET_TABLE_@GOTOFF": "=r" (link_addr) ) ;
	return link_addr ;
}


/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr __attribute__ ((unused))
elf_machine_load_address (void)
{
  /* It doesn't matter what variable this is, the reference never makes
     it to assembly.  We need a dummy reference to some global variable
     via the GOT to make sure the compiler initialized %ebx in time.  */
  Elf32_Addr addr;
  asm ("la	%0, _DYNAMIC@GOTOFF\n" : "=r" (addr) );
  return addr - elf_machine_dynamic() ;
}

#endif


/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */

static inline int __attribute__ ((unused, always_inline))
elf_machine_runtime_setup (struct link_map *l, int lazy, int profile)
{
  Elf32_Addr *got;
  extern void _dl_runtime_resolve (Elf32_Word) attribute_hidden;
  extern void _dl_runtime_profile (Elf32_Word) attribute_hidden;

  if (l->l_info[DT_JMPREL] && lazy)
    {
      /* The GOT entries for functions in the PLT have not yet been filled
	 in.  Their initial contents will arrange when called to push an
	 offset into the .rel.plt section, push _GLOBAL_OFFSET_TABLE_[1],
	 and then jump to _GLOBAL_OFFSET_TABLE[2].  */
      got = (Elf32_Addr *) D_PTR (l, l_info[DT_PLTGOT]);
      /* If a library is prelinked but we have to relocate anyway,
	 we have to be able to undo the prelinking of .got.plt.
	 The prelinker saved us here address of .plt + 0x28.  */
      if (got[1])
	{
	  l->l_mach.plt = got[1] + l->l_addr;
	  l->l_mach.gotplt = (Elf32_Addr) &got[3];
	}
      got[1] = (Elf32_Addr) l;	/* Identify this shared object.  */

      /* The got[2] entry contains the address of a function which gets
	 called to get the address of a so far unresolved function and
	 jump to it.  The profiling extension of the dynamic linker allows
	 to intercept the calls to collect information.  In this case we
	 don't store the address in the GOT so that all future calls also
	 end in this function.  */
      if (__builtin_expect (profile, 0))
	{
	  got[2] = (Elf32_Addr) &_dl_runtime_profile;

	  if (GLRO(dl_profile) != NULL && _dl_name_match_p (GLRO(dl_profile), l))
	    /* This is the object we are looking for.  Say that we really
	       want profiling and the timers are started.  */
	    GL(dl_profile_map) = l;
	}
      else
	/* This function will get called to fix up the GOT entry indicated by
	   the offset on the stack, and then jump to the resolved address.  */
	got[2] = (Elf32_Addr) &_dl_runtime_resolve;
    }

  return lazy;
}

#ifndef ELF_MACHINE_JMP_SLOT
#define ELF_MACHINE_JMP_SLOT    R_NDS32_JMP_SLOT
#endif

#if defined(NDS32_ABI_2) || defined(NDS32_ABI_2FP)
# define STACK_PUSH
# define STACK_POP
# define STACK_PUSH_AUDIT
# define STACK_POP_AUDIT
#else
# define STACK_PUSH	"addi $sp, $sp,	-24"
# define STACK_POP	"addi $sp, $sp,	24"
# define STACK_PUSH_AUDIT     "addi $r19, $r19, -24"
# define STACK_POP_AUDIT      "addi $r19, $r19, 24"
#endif
// #endif /* !dl_machine_h */

// #ifdef RESOLVE_MAP
# define TRAMPOLINE_PROLOG_TEMPLATE(tramp_name) \
asm ("\n\
	.text\n\
	.globl "#tramp_name"\n\
	.type "#tramp_name", #function\n\
	.align 4\n\
"#tramp_name":\n\
	! we get called with\n\
	! 	lp contains the return address from this call\n\
	!	r16 contains offset to target reloc entry\n\
	!	r17 contains GOT[1] (identity of taget lib)\n\
	!  ta is GOT[2] (starting address of this function)\n\
\n\
	! save arguments r0 - r5\n\
	smw.adm	$r0,	[$sp],	$r5,	0\n\
\n\
	! push gp, lp\n\
	smw.adm	$sp,	[$sp],	$sp,	6\n\
\n\
");

#ifdef __NDS32_N1213_43U1H__
# define TRAMPOLINE_GP_TEMPLATE \
asm ("\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+8);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+12);\n\
	add	$gp,	$ta,	$gp;\n\
	");
#else
# define TRAMPOLINE_GP_TEMPLATE  \
asm ("\n\
	mfusr	$ta,	$PC\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+4);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+8);\n\
	add	$gp,	$ta,	$gp;\n\
	");
#endif

# define TRAMPOLINE_BODY_TEMPLATE(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli    $r1,    $r16,   2\n\
	slli    $r16,   $r16,   3\n\
	add     $r1,    $r1,    $r16\n\
	addi	$r2,	$lp,	0\n\
\n\
	! call fixup routine\n\
	bal	"#fixup_name"\n\
\n\
	! save the return\n\
	addi	$ta,	$r0,	0\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	lmw.bim	$r0,	[$sp],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	jr		$r15\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");

// PLTENTER & PLTEXIT version

# define TRAMPOLINE_BODY_TEMPLATE_AUDIT(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli	$r1,	$r16,	2\n\
	slli	$r16,	$r16,	3\n\
	add	$r1,	$r1,	$r16\n\
	addi	$r2,	$lp,	0\n\
	addi	$r3,	$sp,	0\n\
	push	$r3\n\
	smw.adm	$r0,	[$sp],	$r1\n\
	push	$r0\n\
	addi	$r4,	$sp,	0\n\
	push	$r3\n\
\n\
	! call fixup routine\n\
	bal	"#fixup_name"\n\
\n\
	pop	$r3\n\
	! save the return\n\
	addi	$r15,	$r0,	0\n\
	move	$r0,	$sp\n\
	smw.adm	$r18,	[$sp],	$r20\n\
	move	$r20,	$r3\n\
	lwi	$r18,	[$r0]\n\
	bgez	$r18,	1f\n\
	movi	$r18,	0\n\
1:\n\
!	sub	$sp,	$sp,	$r18\n\
	addi	$r19,	$r0,	16\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP_AUDIT"\n\
	lmw.bim	$sp,	[$r19],	$sp,	6\n\
        lmw.bim	$r0,	[$r19],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
!	push	$r18\n\
	jral	$r15\n\
!	pop	$r18\n\
!	add	$sp,	$sp,	$r18\n\
	smw.adm	$r0,	[$sp],	$r1\n\
	addi	$r19,	$sp,	24\n\
	lmw.bim	$r0,	[$r19],	$r1\n\
	move	$r3,	$sp\n\
	! $r3 outregs\n\
	move	$r2,	$r20\n\
	mfusr	$ta,	$PC\n\
	sethi	$gp,	HI20(_GLOBAL_OFFSET_TABLE_+4);\n\
	ori	$gp,	$gp,	LO12(_GLOBAL_OFFSET_TABLE_+8);\n\
	add	$gp,	$ta,	$gp;\n\
	bal	_dl_call_pltexit@PLT\n\
	lmw.bim	$r0,	[$sp],	$r1\n\
	lmw.bim	$r18,	[$sp],	$r20\n\
	addi	$sp,	$sp,	16\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	addi	$sp,	$sp,	8\n\
	lmw.bim	$r2,	[$sp],	$r5,	0\n\
	ret\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");


#if 0
// PLTENTER version
# define TRAMPOLINE_BODY_TEMPLATE_AUDIT(tramp_name, fixup_name) \
asm ("\n\
	! adjust stack\n\
	"STACK_PUSH"\n\
\n\
	! set arguments\n\
	addi	$r0,	$r17,	0\n\
!	addi	$r1,	$r16,	0\n\
	slli    $r1,    $r16,   2\n\
	slli    $r16,   $r16,   3\n\
	add     $r1,    $r1,    $r16\n\
	addi	$r2,	$lp,	0\n\
	addi	$r3,	$sp,	0\n\
	push	$r0\n\
	addi	$r4,	$sp,	0\n\
\n\
	! call fixup routine\n\
	bal	"#fixup_name"\n\
\n\
	! save the return\n\
	addi	$ta,	$r0,	0\n\
	pop	$r0\n\
\n\
	! adjust sp and reload registers\n\
	"STACK_POP"\n\
	lmw.bim	$sp,	[$sp],	$sp,	6\n\
	lmw.bim	$r0,	[$sp],	$r5,	0\n\
\n\
	! jump to the newly found address\n\
	jr		$r15\n\
\n\
	.size "#tramp_name", .-"#tramp_name"\n\
\n\
");
#endif

#ifndef PROF
#ifdef SHARED
#define  ELF_MACHINE_RUNTIME_TRAMPOLINE	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup@PLT);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE_AUDIT (_dl_runtime_profile, _dl_profile_fixup@PLT);
#else
#define  ELF_MACHINE_RUNTIME_TRAMPOLINE	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE_AUDIT (_dl_runtime_profile, _dl_profile_fixup);
#endif
#else
#ifdef SHARED
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup@PLT);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_profile, _dl_fixup@PLT);
#else
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_resolve);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_resolve, _dl_fixup);	\
  TRAMPOLINE_PROLOG_TEMPLATE (_dl_runtime_profile);	\
  TRAMPOLINE_GP_TEMPLATE;	\
  TRAMPOLINE_BODY_TEMPLATE (_dl_runtime_profile, _dl_fixup);
#endif
#endif


/* Mask identifying addresses reserved for the user program,
   where the dynamic linker should not map anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0xf8000000UL

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */
#ifdef __NDS32_N1213_43U1H__
#define RTLD_START asm ("\
	.text\n\
	.globl __start\n\
	.globl _start\n\
	.globl _dl_start_user\n\
	.align	4\n\
1:\n\
	ret\n\
__start:\n\
_start:\n\
	! we are PIC code, so get global offset table\n\
	jal	1b\n\
	sethi	$gp, HI20(_GLOBAL_OFFSET_TABLE_)\n\
	ori	$gp, $gp, LO12(_GLOBAL_OFFSET_TABLE_+4)\n\
	add	$gp, $lp, $gp\n\
\n\
	! at start time, all the args are on the stack\n\
	addi	$r0,	$sp,	0\n\
   ! adjust stack\n\
   "STACK_PUSH"\n\
	bal	_dl_start@PLT\n\
	! save user entry point in r6 which is callee saved\n\
	addi	$r6,	$r0,	0\n\
   ! adjust sp and reload registers\n\
   "STACK_POP"\n\
\n\
_dl_start_user:\n\
	! See if we were run as a command with the executable file\n\
	! name as an extra leading argument.\n\
	! skip these arguments\n\
	l.w	$r2,	_dl_skip_args@GOTOFF	! args to skip\n\
	lwi	$r0,	[$sp+0]	! original argc\n\
	slli	$r1,	$r2,	2	! offset for new sp\n\
	add	$sp,	$sp,	$r1	! adjust sp to skip args\n\
	sub	$r0,	$r0,	$r2	! set new argc\n\
	swi	$r0,	[$sp+0]	! save new argc\n\
\n\
!"RTLD_START_SPECIAL_INIT"\n\
	! prepare args to call _dl_init\n\
	l.w	$r0,	_rtld_local@GOTOFF\n\
	lwi	$r1,	[$sp+0]	! argc\n\
	addi	$r2,	$sp,	4	! argv\n\
	slli	$r3,	$r1,	2	! envp = sp + argc * 4 + 8\n\
	addi	$r3,	$r3,	8\n\
	add	$r3,	$r3,	$sp	! envp\n\
	!pushm	$r0,	$r5,	$sp	! push vars\n\
	"STACK_PUSH"\n\
	bal	_dl_init_internal@PLT\n\
	"STACK_POP"\n\
\n\
	! load address of _dl_fini finalizer function\n\
	la		$r5, _dl_fini@GOTOFF\n\
	! jump to the user_s entry point\n\
   addi    $r15,   $r6, 0 \n\
	jr	$r15\n\
	.previous\n\
");
#else
#define RTLD_START asm ("\
	.text\n\
	.globl __start\n\
	.globl _start\n\
	.globl _dl_start_user\n\
	.align	4\n\
__start:\n\
_start:\n\
	! we are PIC code, so get global offset table\n\
	mfusr $r15, $PC \n\
	sethi	$gp, HI20(_GLOBAL_OFFSET_TABLE_ + 4)\n\
	ori	$gp, $gp, LO12(_GLOBAL_OFFSET_TABLE_ + 8)\n\
	add	$gp, $r15, $gp\n\
\n\
	! at start time, all the args are on the stack\n\
	addi	$r0,	$sp,	0\n\
   ! adjust stack\n\
   "STACK_PUSH"\n\
	bal	_dl_start@PLT\n\
	! save user entry point in r6 which is callee saved\n\
	addi	$r6,	$r0,	0\n\
   ! adjust sp and reload registers\n\
   "STACK_POP"\n\
\n\
_dl_start_user:\n\
	! See if we were run as a command with the executable file\n\
	! name as an extra leading argument.\n\
	! skip these arguments\n\
	l.w	$r2,	_dl_skip_args@GOTOFF	! args to skip\n\
	lwi	$r0,	[$sp+0]	! original argc\n\
	slli	$r1,	$r2,	2	! offset for new sp\n\
	add	$sp,	$sp,	$r1	! adjust sp to skip args\n\
	sub	$r0,	$r0,	$r2	! set new argc\n\
	swi	$r0,	[$sp+0]	! save new argc\n\
\n\
!"RTLD_START_SPECIAL_INIT"\n\
	! prepare args to call _dl_init\n\
	l.w	$r0,	_rtld_local@GOTOFF\n\
	lwi	$r1,	[$sp+0]	! argc\n\
	addi	$r2,	$sp,	4	! argv\n\
	slli	$r3,	$r1,	2	! envp = sp + argc * 4 + 8\n\
	addi	$r3,	$r3,	8\n\
	add	$r3,	$r3,	$sp	! envp\n\
	!pushm	$r0,	$r5,	$sp	! push vars\n\
	"STACK_PUSH"\n\
	bal	_dl_init_internal@PLT\n\
	"STACK_POP"\n\
\n\
	! load address of _dl_fini finalizer function\n\
	la		$r5, _dl_fini@GOTOFF\n\
	! jump to the user_s entry point\n\
	jr	$r6\n\
	.previous\n\
");
#endif

#ifndef RTLD_START_SPECIAL_INIT
#define RTLD_START_SPECIAL_INIT /* nothing */
#endif

# define elf_machine_type_class(type) \
  ((((type) == R_NDS32_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)			      \
   | (((type) == R_NDS32_COPY) * ELF_RTYPE_CLASS_COPY))


/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_NDS32_JMP_SLOT

/* We define an initialization functions.  This is called very early in
   _dl_sysdep_start.  */
#define DL_PLATFORM_INIT dl_platform_init ()

static inline void __attribute__ ((unused))
dl_platform_init (void)
{
  if (GLRO(dl_platform) != NULL && *GLRO(dl_platform) == '\0')
    /* Avoid an empty string which would disturb us.  */
    GLRO(dl_platform) = NULL;
}

/* Fixup a PLT entry to bounce directly to the function at VALUE.  */
static inline Elf32_Addr
elf_machine_fixup_plt (struct link_map *map, lookup_t t,
		       const Elf32_Rel *reloc,
		       Elf32_Addr *reloc_addr, Elf32_Addr value)
{
  return *reloc_addr = value;
}

static inline Elf32_Addr
elf_machine_plt_value (struct link_map *map, const Elf32_Rela *reloc,
		       Elf32_Addr value)
{
  return value + reloc->r_addend ;
}

// old #endif /* !dl_machine_h */


// shared.
/* Names of the architecture-specific auditing callback functions.  */
#define ARCH_LA_PLTENTER nds32_gnu_pltenter
#define ARCH_LA_PLTEXIT nds32_gnu_pltexit

#endif /* !dl_machine_h */
// #endif /* RESOLVE_MAP  */


#define ELF_MACHINE_NO_REL 1
#ifdef RESOLVE_MAP


/* NDS32 never uses Elf32_Rel relocations.	 */
#define ELF_MACHINE_NO_REL 1

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

auto inline void
elf_machine_rela (struct link_map *map, const ElfW(Rela) *reloc,
                  const ElfW(Sym) *sym, const struct r_found_version *version,
                  void *const reloc_addr_arg, int skip_ifunc)
{

  Elf32_Addr *const reloc_addr = reloc_addr_arg;
  const unsigned int r_type = ELF32_R_TYPE (reloc->r_info);
  Elf32_Addr value;

#define COPY_UNALIGNED_WORD(swp, twp, align) \
  { \
    void *__s = (swp), *__t = (twp); \
    unsigned char *__s1 = __s, *__t1 = __t; \
    unsigned short *__s2 = __s, *__t2 = __t; \
    unsigned long *__s4 = __s, *__t4 = __t; \
    switch ((align)) \
    { \
    case 0: \
      *__t4 = *__s4; \
      break; \
    case 2: \
      *__t2++ = *__s2++; \
      *__t2 = *__s2; \
      break; \
    default: \
      *__t1++ = *__s1++; \
      *__t1++ = *__s1++; \
      *__t1++ = *__s1++; \
      *__t1 = *__s1; \
      break; \
    } \
  }

  if (__builtin_expect (r_type == R_NDS32_RELATIVE, 0))
    {
#ifndef RTLD_BOOTSTRAP
      if (map != &GL(dl_rtld_map)) /* Already done in rtld itself.	 */
#endif
	{
	  if (reloc->r_addend)
	    value = map->l_addr + reloc->r_addend;
	  else
	    {
	      COPY_UNALIGNED_WORD (reloc_addr_arg, &value,
				   (int) reloc_addr_arg & 3);
	      value += map->l_addr;
	    }
	  COPY_UNALIGNED_WORD (&value, reloc_addr_arg,
			       (int) reloc_addr_arg & 3);
	}
    }
#ifndef RTLD_BOOTSTRAP
  else if (__builtin_expect (r_type == R_NDS32_NONE, 0))
    return;
#endif
  else
    {
      const Elf32_Sym *const refsym = sym;
      struct link_map *sym_map = RESOLVE_MAP (&sym, version, r_type);
      value = sym_map == NULL ? 0 : sym_map->l_addr;


# ifndef RTLD_BOOTSTRAP
      if (sym != NULL)
# endif
      value += sym->st_value;
      value += reloc->r_addend;

      switch (r_type)
	{
	case R_NDS32_COPY:
	  if (sym == NULL)
	    /* This can happen in trace mode if an object could not be
	       found.  */
	    break;
	  if (sym->st_size > refsym->st_size
	      || (sym->st_size < refsym->st_size && GLRO(dl_verbose)))
	    {
	      const char *strtab;

	      strtab = (const char *) D_PTR (map, l_info[DT_STRTAB]);
	      _dl_error_printf ("\
%s: Symbol `%s' has different size in shared object, consider re-linking\n",
				rtld_progname ?: "<program name unknown>",
				strtab + refsym->st_name);
	    }
	  memcpy (reloc_addr_arg, (void *) value,
		  MIN (sym->st_size, refsym->st_size));
	  break;
	case R_NDS32_GLOB_DAT:
	case R_NDS32_JMP_SLOT:
	case R_NDS32_32:
	  /* These addresses are always aligned.  */
	  *reloc_addr = value;
	  break;
        case R_NDS32_32_RELA:
	  COPY_UNALIGNED_WORD (&value, reloc_addr,
			       (int) reloc_addr & 3);
	  break;
	case R_NDS32_NONE:
          break;
	default:
	  _dl_reloc_bad_type (map, r_type, 0);
	  break;
	}
    }
}

auto inline void
__attribute__ ((always_inline))
elf_machine_rela_relative (Elf32_Addr l_addr, const Elf32_Rela *reloc,
			   void *const reloc_addr_arg)
{
  Elf32_Addr value;

  if (reloc->r_addend)
    value = l_addr + reloc->r_addend;
  else
    {
      COPY_UNALIGNED_WORD (reloc_addr_arg, &value, (int) reloc_addr_arg & 3);
      value += l_addr;
    }
  COPY_UNALIGNED_WORD (&value, reloc_addr_arg, (int) reloc_addr_arg & 3);

#undef COPY_UNALIGNED_WORD
}

auto inline void
__attribute__ ((always_inline))
elf_machine_lazy_rel (struct link_map *map,
		      Elf32_Addr l_addr, const Elf32_Rela *reloc, int skip_ifunc)
{
  Elf32_Addr *const reloc_addr = (void *) (l_addr + reloc->r_offset);
  /* Check for unexpected PLT reloc type.  */
  if (ELF32_R_TYPE (reloc->r_info) == R_NDS32_JMP_SLOT)
    {
      if (__builtin_expect (map->l_mach.plt, 0) == 0)
	*reloc_addr += l_addr;
      else
	*reloc_addr =
	  map->l_mach.plt
	  + (((Elf32_Addr) reloc_addr) - map->l_mach.gotplt) * 6;
    }
  else if (ELF32_R_TYPE (reloc->r_info) == R_NDS32_NONE) {
    }
  else
    _dl_reloc_bad_type (map, ELF32_R_TYPE (reloc->r_info), 1);
}
#endif /* RESOLVE_MAP  */

