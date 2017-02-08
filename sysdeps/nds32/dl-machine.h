/* Machine-dependent ELF dynamic relocation inline functions.  NDS32 version.
   Copyright (C) 2013-2014 Free Software Foundation, Inc.
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

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "NDS32"

#include <tls.h>
#include <dl-tlsdesc.h>
#include <linkmap.h>
#include <ldsodefs.h>

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int __attribute__ ((unused))
elf_machine_matches_host(const Elf32_Ehdr *ehdr)
{
  return ehdr->e_machine == EM_NDS32;
}


/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT, a special entry that is never relocated.  */
static inline Elf32_Addr __attribute__ ((unused, const))
elf_machine_dynamic (void)
{
  /* This produces a GOTOFF reloc that resolves to some offset at link time, so in
     fact just loads from the GOT register directly.  By doing it without
     an asm we can let the compiler choose any register.  */
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



/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */

static inline int __attribute__ ((unused, always_inline))
elf_machine_runtime_setup (struct link_map *l, int lazy, int profile)
{

  if (l->l_info[DT_JMPREL] && lazy)
    {
      Elf32_Addr *got;
      extern void _dl_runtime_resolve (Elf32_Word) attribute_hidden;
      extern void _dl_runtime_profile (Elf32_Word) attribute_hidden;
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

  if (l->l_info[ADDRIDX (DT_TLSDESC_GOT)] && lazy)
    *(Elf32_Addr*)(D_PTR (l, l_info[ADDRIDX (DT_TLSDESC_GOT)]) + l->l_addr)
      = (Elf32_Addr) &_dl_tlsdesc_resolve_rela;
  return lazy;
}

#ifndef ELF_MACHINE_JMP_SLOT
#define ELF_MACHINE_JMP_SLOT    R_NDS32_JMP_SLOT
#endif


/* Mask identifying addresses reserved for the user program,
   where the dynamic linker should not map anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0xf8000000UL

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */
#define RTLD_START asm ("						\
	.text								\n\
	.globl __start							\n\
	.globl _start							\n\
	.globl _dl_start_user						\n\
	.align	4							\n\
__start:								\n\
_start:									\n\
	! we are PIC code, so get global offset table			\n\
	mfusr 	$r15, $PC 						\n\
	sethi	$gp, HI20(_GLOBAL_OFFSET_TABLE_ + 4)			\n\
	ori	$gp, $gp, LO12(_GLOBAL_OFFSET_TABLE_ + 8)		\n\
	add	$gp, $r15, $gp						\n\
									\n\
	! at start time, all the args are on the stack			\n\
	addi	$r0,	$sp,	0					\n\
	bal	_dl_start@PLT						\n\
	! save user entry point in r6 which is callee saved		\n\
	addi	$r6,	$r0,	0					\n\
									\n\
_dl_start_user:								\n\
	! See if we were run as a command with the executable file	\n\
	! name as an extra leading argument.				\n\
	! skip these arguments						\n\
	l.w	$r2,	_dl_skip_args@GOTOFF	! args to skip		\n\
	bnez	$r2,	2f						\n\
!"RTLD_START_SPECIAL_INIT"						\n\
	! prepare args to call _dl_init					\n\
	addi	$r2,	$sp,	4		! argv			\n\
1:									\n\
	l.w	$r0,	_rtld_local@GOTOFF				\n\
	lwi	$r1,	[$sp+0]			! argc			\n\
	slli	$r3,	$r1,	2		! envp =sp +argc * 4 + 8\n\
	addi	$r3,	$r3,	8					\n\
	add	$r3,	$r3,	$sp		! envp			\n\
	bal	_dl_init@PLT						\n\
									\n\
	! load address of _dl_fini finalizer function			\n\
	la		$r5, _dl_fini@GOTOFF				\n\
	! jump to the user_s entry point				\n\
	jr	$r6							\n\
2:									\n\
	lwi	$r0,	[$sp+0]			! original argc		\n\
	slli	$r1,	$r2,	2		! offset for new sp	\n\
	add	$sp,	$sp,	$r1		! adjust sp to skip args\n\
	sub	$r0,	$r0,	$r2		! set new argc		\n\
	swi	$r0,	[$sp+0]			! save new argc		\n\
	andi	$r0,	$sp,	7					\n\
	beqz	$r0,	1b						\n\
									\n\
	! Make stack 8-byte aligned					\n\
	bitci	$sp,	$sp,	7					\n\
	move	$r2,	$sp						\n\
3:	! argv								\n\
	lwi	$r0,	[$r2+4]						\n\
	smw.bim $r0,[$r2],$r0,#0					\n\
	bnez	$r0,	3b						\n\
									\n\
3:	! envp								\n\
	lwi	$r0,	[$r2+4]						\n\
	smw.bim $r0,[$r2],$r0,#0					\n\
	bnez	$r0,	3b						\n\
									\n\
3:	! auxv								\n\
	lmw.ai	$r0,[$r2],$r1,#0					\n\
	smw.bim	$r0,[$r2],$r1,#0					\n\
	bnez	$r0,	3b						\n\
									\n\
	!update _dl_argv						\n\
	addi	$r2,	$sp,	4					\n\
	s.w	$r2,	_dl_argv@GOTOFF		! args to skip		\n\
	j	1b							\n\
	.previous							\n\
");

#ifndef RTLD_START_SPECIAL_INIT
#define RTLD_START_SPECIAL_INIT /* nothing */
#endif

# define elf_machine_type_class(type) \
  ((((type) == R_NDS32_JMP_SLOT) \
	|| ((type)== R_NDS32_TLS_TPOFF) \
	|| ((type)== R_NDS32_TLS_DESC) \
	 * ELF_RTYPE_CLASS_PLT)			      \
   | (((type) == R_NDS32_COPY) * ELF_RTYPE_CLASS_COPY) \
   | (((type) == R_NDS32_GLOB_DAT) * ELF_RTYPE_CLASS_EXTERN_PROTECTED_DATA))


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
		       const Elf32_Rela *reloc,
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

/* Names of the architecture-specific auditing callback functions.  */
#define ARCH_LA_PLTENTER nds32_gnu_pltenter
#define ARCH_LA_PLTEXIT nds32_gnu_pltexit

#endif /* !dl_machine_h */

/* NDS32 uses Elf32_Rela no Elf32_Rel */
#define ELF_MACHINE_NO_REL 1
#define ELF_MACHINE_NO_RELA 0

#ifdef RESOLVE_MAP

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

#define COPY_UNALIGNED_WORD(swp, twp) \
  { \
    unsigned int __tmp = __builtin_nds32_unaligned_load_w (swp); \
    __builtin_nds32_unaligned_store_w (twp, __tmp); \
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
	      COPY_UNALIGNED_WORD (reloc_addr_arg, &value);
	      value += map->l_addr;
	    }
	  COPY_UNALIGNED_WORD (&value, reloc_addr_arg);
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
	  COPY_UNALIGNED_WORD (&value, reloc_addr);
	  break;
	case R_NDS32_NONE:
          break;
	case R_NDS32_TLS_TPOFF:
	{
#  ifdef RTLD_BOOTSTRAP
		*reloc_addr = map->l_tls_offset + sym->st_value + reloc->r_addend;
#  else
		if (sym != NULL)
		{
			CHECK_STATIC_TLS (map, sym_map);
			*reloc_addr = sym_map->l_tls_offset + sym->st_value + reloc->r_addend;
		}
		else
			_dl_error_printf ("sym is NULL in R_NDS32_TLS_TPOFF\n");
# endif
	}
	break;

	case R_NDS32_TLS_DESC:
	{
	    struct tlsdesc volatile *td =
	      (struct tlsdesc volatile *)reloc_addr;
# ifndef RTLD_BOOTSTRAP
		if(!sym)
		{
			td->argument.value = reloc->r_addend;
			td->entry = _dl_tlsdesc_undefweak;
		}
		else
# endif
		{
			value=sym->st_value + reloc->r_addend;
#   ifndef RTLD_BOOTSTRAP
#    ifndef SHARED
			CHECK_STATIC_TLS (map, sym_map);
#    else
			if (!TRY_STATIC_TLS (map, sym_map))
		  	{
		    		td->argument.pointer = _dl_make_tlsdesc_dynamic(sym_map, value);
		    		td->entry = _dl_tlsdesc_dynamic;
		  	}
			else
#    endif
#   endif
		  	{
		    		td->argument.value = value + sym_map->l_tls_offset;
		    		td->entry = _dl_tlsdesc_return;
		  	}
		}
	}
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
      COPY_UNALIGNED_WORD (reloc_addr_arg, &value);
      value += l_addr;
    }
  COPY_UNALIGNED_WORD (&value, reloc_addr_arg);

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
  else if (ELF32_R_TYPE (reloc->r_info) == R_NDS32_TLS_DESC)
    {
      struct tlsdesc volatile *td =
	(struct tlsdesc volatile *)reloc_addr;

      td->argument.pointer = (void*)reloc;
      td->entry = (void*)(D_PTR (map, l_info[ADDRIDX (DT_TLSDESC_PLT)])
			  + map->l_addr);
    }
  else if (ELF32_R_TYPE (reloc->r_info) == R_NDS32_NONE) {
    }
  else
    _dl_reloc_bad_type (map, ELF32_R_TYPE (reloc->r_info), 1);
}
#endif /* RESOLVE_MAP  */

