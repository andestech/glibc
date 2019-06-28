/* Machine-dependent program header inspection for the ELF loader.
   Copyright (C) 2014-2018 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _DL_MACHINE_REJECT_PHDR_H
#define _DL_MACHINE_REJECT_PHDR_H 1

#include <unistd.h>
#include <strings.h>
#include <sys/prctl.h>

/* Reject an object with a debug message.  */
#define REJECT(str, args ...)                                                  \
  {                                                                           \
    if (__glibc_unlikely (GLRO(dl_debug_mask) & DL_DEBUG_LIBS))               \
      _dl_debug_printf (str, ## args);                                         \
  }

/* Search the program headers for the ABI Flags.  */
static inline const ElfW(Phdr) *
find_riscv_flags (const ElfW(Phdr) *phdr, ElfW(Half) phnum)
{
  const ElfW(Phdr) *ph;

  for (ph = phdr; ph < &phdr[phnum]; ++ph)
    if (ph->p_type == PT_RISCV_ATTRIBUTE)
      return ph;
  return NULL;
}

static inline bool
my_isdigit(char ch)
{
  if (ch >= '0' && ch <= '9' )
    return true;
  return false;
}

static inline char
my_tolower(char ch)
{
  if (ch >= 'A' && ch <= 'Z' )
    return ch-'A'+'a';
  return ch;
}

static const char
*strcasechr(const char *str, int c)
{
  const char *s = str;
  for (; my_tolower(*s) != my_tolower((char)c); s++)
    if (*s == '\0')
      return NULL;
  return (const char *)s;
}

char
parse_byte(const char** buf, const char* end)
{
  const char ch = **buf;

  if (end - *buf < sizeof(char))
    REJECT("Truncated ELF attribute subsection\n");
  *buf = *buf + sizeof(char);
  return ch;
}

unsigned int
parse_uint32le(const char** buf, const char* end)
{
  const unsigned int i = *((const unsigned int*) *buf);

  if (end - *buf < sizeof(unsigned int))
    REJECT("Truncated ELF attribute subsection\n");
  *buf = *buf + sizeof(unsigned int);
  return i;
}

const char*
parse_ntbs(const char** buf, const char* end)
{
  const char* s = (const char*) *buf;
  while (*buf < end && **buf != '\0')
    (*buf)++;

  if (*buf == end)
    REJECT("Truncated ELF attribute subsection\n");

  (*buf)++;
  return s;
}

unsigned long
parse_uleb128(const char** buf, const char* end)
{
  unsigned long shift = 0;
  unsigned long value = 0;

  while (*buf < end) {
      value |= (**buf & 0x7f) << shift;
      if ((**buf & 0x80) == 0)
        break;
      *buf += 1;
      shift += 7;
    }

  if (*buf == end)
    REJECT("Truncated ELF attribute subsection\n");

  *buf += 1;

  return value;
}

int
parse_decimal(const char** s)
{
  int n = 0;

  if (!my_isdigit(**s))
    REJECT("Error parsing decimal number\n");

  while (my_isdigit(**s)) {
      n = n * 10 + (**s - '0');
      *s += 1;
    }

  return n;
}

struct riscv_version
{
  int major;
  int minor;
};

/* Retrieve the version info and move the pointerto the next part.  */
static bool
parse_riscv_isa_version(const char** ver)
{
  parse_decimal(ver);

  if (**ver != 'p')
    {
      REJECT("Major version number is not followed by `p'\n");
      return false;
    }
  *ver += 1;

  return true;
}


static inline void
skip_underscores(const char **s)
{
  while (**s == '_')
    (*s)++;
}

static bool
riscv_isa_version_backward_compatible(struct riscv_version *elf,
                                      struct riscv_version *host)
{
  if(elf->major > host->major)
    return false;
  if(elf->major == host->major || elf->minor > host->minor)
    return false;
  return true;
}

static bool
riscv_isa_version_exact_compatible(struct riscv_version *elf,
                                   struct riscv_version *host)
{
  if(elf->major != host->major || elf->minor != host->minor)
    return false;
  return true;
}

typedef bool (*compat_func_t)(struct riscv_version *elf,
                              struct riscv_version *host);

static bool
riscv_isa_version_compatible(const char** elf_ver,
                             const char **host_ver, compat_func_t fn)
{
  struct riscv_version elf;
  struct riscv_version host;

  /* ISA version is in the format of "<major>p<minor>" where major and
     minor numbers are decimal. We don't use strtol() here to avoid
     accepting whitespaces or plus/minus signs.  */
  elf.major = parse_decimal(elf_ver);
  host.major = parse_decimal(host_ver);

  if (**elf_ver != 'p' || **host_ver != 'p')
    {
      REJECT("Major version number is not followed by `p'\n");
      return false;
    }
  *elf_ver += 1;
  *host_ver += 1;

  elf.minor = parse_decimal(elf_ver);
  host.minor = parse_decimal(host_ver);

  return (*fn)(&elf, &host);
}

static bool riscv_non_standard_compatible(const char **elf_nse,
                                          const char **host_nse)
{
  /* We need more discussion on non-standard extension processing.  */
  return true;
}

const char* riscv_extensions = "MAFDQLCBJTPVN";

static inline bool
extension_later_than(char elf_ex, char host_ex)
{
  const char* elf = strcasechr(riscv_extensions, elf_ex);
  const char* host = strcasechr(riscv_extensions, host_ex);

  if (elf > host)
    return true;
  return false;
}

static inline bool
reach_extension_end(char host_ex)
{
  if (host_ex == 'x' || host_ex == 'X' || host_ex == '\0')
    return true;
  return false;
}

static bool
riscv_extension_compatible(const char** elf_isa,
                           const char **host_isa)
{
  char ch;
  char *prev_pos = NULL;
  char *curr_pos;

  while (**elf_isa != '\0') {
      if (**elf_isa == 'x' || **elf_isa == 'X')
        return true;

      while (extension_later_than(**elf_isa, **host_isa)) {
          (*host_isa)++;
          parse_riscv_isa_version(host_isa);

          if (reach_extension_end(**host_isa))
            return false;
        }

      curr_pos = (char*)strcasechr(riscv_extensions, **elf_isa);
      if (prev_pos && prev_pos > curr_pos)
        {
          REJECT("Wrong extension order\n");
          return false;
        }
      prev_pos = curr_pos;

      if (my_tolower(**elf_isa) < my_tolower(**host_isa))
        {
          /* According to the law of trichonomy, something went wrong here.
           * This ELF has an non-supported extension. Abort the check.  */
          REJECT("Invalid extension `%c'\n", **elf_isa);
          return false;
        }

      ch = **elf_isa;
      switch (my_tolower(ch)) {
        /* This group of extensions has been frozen,
           so an exact match is assumed.  */
        case 'm':
        case 'a':
        case 'f':
        case 'd':
        case 'q':
        case 'c':
          (*elf_isa)++;
          (*host_isa)++;
          if (!riscv_isa_version_compatible(elf_isa, host_isa,
                                            &riscv_isa_version_exact_compatible))
            {
              REJECT("Extension `%c' not compatible\n", ch);
              return false;
            }
          break;

        /* Extensions in this group are development in progress.
         * Assuming backward compatibility here.  */
        case 'l':
        case 'b':
        case 'j':
        case 't':
        case 'p':
        case 'v':
        case 'n':
          (*elf_isa)++;
          (*host_isa)++;
          if (!riscv_isa_version_compatible(elf_isa, host_isa,
                                            &riscv_isa_version_backward_compatible))
            {
              REJECT("Extension `%c' not compatible\n", ch);
              return false;
            }
          break;

        default:
          /* Should NOT be here.  */
          REJECT("Invalid extension `%c'\n", ch);
          return false;
        }

      skip_underscores(elf_isa);
      skip_underscores(host_isa);
    }
  return true;
}

#define RISCV_NUM_BASE_ISA 4
static bool
riscv_base_isa_compatible(const char** elf_isa, const char **host_isa)
{
  /* The ISA string must begin with one of these four.  */
  const char* base_isas[RISCV_NUM_BASE_ISA] = { "rv32i", "rv32e", "rv64i",
                                                "rv128i" };
  int elf_base = -1;
  int host_base = -1;
  int i;

  for (i = 0; i < RISCV_NUM_BASE_ISA; i++) {
      size_t len = strlen(base_isas[i]);
      if (elf_base < 0 && !strncmp(*elf_isa, base_isas[i], len))
        {
          elf_base = i;
          *elf_isa += len;
        }
      if (host_base < 0 && !strncmp(*host_isa, base_isas[i], len))
        {
          host_base = i;
          *host_isa += len;
        }
    }

  if (elf_base < 0 || host_base < 0)
    goto out_invalid_base;

  if (elf_base != host_base)
    goto out_invalid_base;

  if (!riscv_isa_version_compatible(elf_isa, host_isa,
                                    &riscv_isa_version_exact_compatible))
    {
      REJECT("Base ISA version not compatible\n");
      return false;
    }

  skip_underscores(elf_isa);
  skip_underscores(host_isa);

  return true;
out_invalid_base:
  REJECT("Invalid base ISA\n");
  return false;
}

static bool
riscv_isa_compatible(const char* elf_isa)
{
  const char *host_isa = GLRO(dl_platform);

  if (!strcmp(elf_isa, host_isa))
    return true;

  /* The ISA string starts with the base ISA.  */
  if (!riscv_base_isa_compatible(&elf_isa, &host_isa))
    return false;
  
  /* Followed by multiple extensions.  */
  if (!riscv_extension_compatible(&elf_isa, &host_isa))
    return false;

  /* Optionally followed by non-standard extensions.  */
  if (!riscv_non_standard_compatible(&elf_isa, &host_isa))
    return false;

  return true;
}

static bool riscv_priv_spec_compatible(struct riscv_version *priv)
{
  struct riscv_version host;

  host.major = GLRO(dl_hwcap2) >> 16;
  host.minor = GLRO(dl_hwcap2) & 0xFFFF;
  if (!riscv_isa_version_exact_compatible(priv, &host))
    {
      REJECT("Privileged Spec version %u.%u not compatible\n", priv->major, priv->minor);
      return false;
    }
  return true;
}

#define Tag_File               1
#define Tag_RISCV_stack_align        4
#define Tag_RISCV_arch               5
#define Tag_RISCV_unaligned_access   6
#define Tag_RISCV_priv_spec          8
#define Tag_RISCV_priv_spec_minor    10
#define Tag_RISCV_priv_spec_revision 12
#define Tag_ict_version              0x8000
#define Tag_ict_model                0x8001

#define Tag_arch_legacy               4
#define Tag_priv_spec_legacy          5
#define Tag_priv_spec_minor_legacy    6
#define Tag_priv_spec_revision_legacy 7
#define Tag_strict_align_legacy       8
#define Tag_stack_align_legacy        9

static int
parse_riscv_attributes(const char* buf, const char* end)
{
  unsigned long tag;
  const char* isa;
  struct riscv_version priv;
  int total_check = 0;

  priv.major = 0;
  priv.minor = 0;

  while (buf < end) {
      /* Each attribute is a pair of tag and value. The value can be
         either a null-terminated byte string or an ULEB128 encoded
         integer depending on the tag.  */
      tag = parse_uleb128(&buf, end);
      switch (tag) {
        case Tag_RISCV_stack_align:
          parse_uleb128(&buf, end);
          break;

        case Tag_RISCV_arch:
          /* For Tag_arch, parse the arch substring.  */
          isa = parse_ntbs(&buf, end);
          if (!riscv_isa_compatible(isa)){
            return ENOEXEC;
          }
          total_check++;
          break;

        case Tag_RISCV_priv_spec:
          priv.major = parse_uleb128(&buf, end);
          total_check++;
          break;

        case Tag_RISCV_priv_spec_minor:
          priv.minor = parse_uleb128(&buf, end);
          total_check++;
          break;

        /* Simply ignore other tags.  */
        case Tag_RISCV_priv_spec_revision:
        case Tag_RISCV_unaligned_access:
          continue;

        default:
          REJECT("Unknown RISCV Attribute Tag %lu\n", tag);
          continue;
        }

      if (total_check == 3)
        {
          if (riscv_priv_spec_compatible(&priv))
            return 0;
          break;
        }
    }
  return ENOEXEC;
}

static int
parse_legacy_riscv_attributes(const char* buf, const char* end)
{
  unsigned long tag;
  const char* isa;
  struct riscv_version priv;
  int total_check = 0;

  priv.major = 0;
  priv.minor = 0;

  while (buf < end) {
      /* Each attribute is a pair of tag and value. The value can be
         either a null-terminated byte string or an ULEB128 encoded
         integer depending on the tag.  */
      tag = parse_uleb128(&buf, end);
      switch (tag) {
        case Tag_arch_legacy:
          /* For Tag_arch, parse the arch substring.  */
          isa = parse_ntbs(&buf, end);
          if (!riscv_isa_compatible(isa)){
            return ENOEXEC;
          }
          total_check++;
          break;

        case Tag_priv_spec_legacy:
          priv.major = parse_uleb128(&buf, end);
          total_check++;
          break;

        case Tag_priv_spec_minor_legacy:
          priv.minor = parse_uleb128(&buf, end);
          total_check++;
          break;

        /* Simply ignore other tags.  */
        case Tag_priv_spec_revision_legacy:
        case Tag_strict_align_legacy:
        case Tag_stack_align_legacy:
          continue;

        default:
          REJECT("Unknown RISCV Attribute Tag %lu\n", tag);
          continue;
        }

      if (total_check == 3)
        {
          if (riscv_priv_spec_compatible(&priv))
            return 0;
          break;
        }
    }
  return ENOEXEC;
}

static bool is_legacy_riscv_attributes(const char* buf, const char* end)
{            
  unsigned long tag = parse_uleb128(&buf, end);
             
  if (tag == 4 && end - buf >= 2 && !strncmp((const char*) buf, "rv", 2))
    {
      return true;
    }
  return false;
}

/* We assume that the subsection started with "riscv", the only subsection we
   support now, has one attribute entry only.  We don't support finer
   grainularity (attributes for multiple files, sections or symbols), but
   recongnize the ELF file as a whole.  */
static int
parse_riscv_subsection(const char* buf, const char* end)
{
  const char* sub_begin = buf;
  const char* sub_end;
  unsigned int len;

  /* The "riscv" subsection must begin with a Tag_File.  */
  if (parse_uleb128(&buf, end) != Tag_File)
    {
      REJECT("No Tag_File in \"riscv\" subsection\n");
      goto out_noexec;
    }

  /* Followed by a length field including the tag byte.  */
  len = parse_uint32le(&buf, end);
  if (end - sub_begin < len)
    {
      REJECT("Truncated ELF attribute subsection\n");
      goto out_noexec;
    }
  sub_end = sub_begin + len;

  /* Followed by the actual RISC-V attributes.  */
  if (is_legacy_riscv_attributes(buf, sub_end))
    {
      if (parse_legacy_riscv_attributes(buf, sub_end))
      goto out_noexec;
    } 
  else if (parse_riscv_attributes(buf, sub_end))
      goto out_noexec;

  return 0;
out_noexec:
  return ENOEXEC;
}

/* We implement this ELF attribute section with reference to Sec 2.2, ARM IHI
   0045E specification, but a slightly restricted form.  For detail please
   check parse_elf_attribute_subsection.  */
static int
parse_elf_attribute_section(const char* buf, const char* end)
{
  const char version = parse_byte(&buf, end);
  char* sub_begin;
  char* sub_end;
  size_t len;

  /* The first byte must be the ASCII character 'A'.  */
  if (version != 'A')
    {
      REJECT("Unsupported ELF attribute version `%c'\n", version);
      goto out_noexec;
    }

  /* The section is divided into multiple subsections.  */
  while (buf < end) {
      /* Each subsection begins with a 32-bit unsigned integer
         indicating its length (including the length field itself).  */
      sub_begin = (char *)buf;
      len = parse_uint32le(&buf, end);
      if (end - sub_begin < len)
        {
          REJECT("Truncated ELF attribute subsection\n");
          goto out_noexec;
        }
      sub_end = sub_begin + len;

      /* It is followed by the a null-terminated name which determines
         how to parse the content.  For now we only support one
         subsection named "riscv" here.  */
      if (strcmp(buf, "riscv"))
        {
          buf = sub_end;
          continue;
        }

      buf += strlen("riscv") + 1;
      if(parse_riscv_subsection(buf, sub_end))
        goto out_noexec;

      buf = sub_end;
    }

  return 0;
out_noexec:
  return ENOEXEC;
}

static bool __attribute_used__
elf_machine_reject_phdr_p (const ElfW(Phdr) *phdr, uint_fast16_t phnum,
                           const char *buf, size_t len, struct link_map *map,
                           int fd)
{
  const ElfW(Phdr) *ph = find_riscv_flags (phdr, phnum);
  char *rvattr_seg = NULL;

  /* The toolchain supports attributes section.  */
  if (ph != NULL && GLRO(dl_platform) && GLRO(dl_hwcap))
    {
      ElfW(Addr) size = ph->p_filesz;

      rvattr_seg= alloca (size);
      __lseek (fd, ph->p_offset, SEEK_SET);
      if (__libc_read (fd, (void *) rvattr_seg, size) != size)
        REJECT ("unable to read PT_RISCV_ATTRIBUTE\n");

      if(parse_elf_attribute_section(rvattr_seg, rvattr_seg + size))
        return true;
      
    }

  return false;
}

#endif /* dl-machine-reject-phdr.h */
