/* Optimization of multithreaded code.

   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

/* Specification.  */
#include "thread-optim.h"

static int mt_override = -1;

void
gl_set_multithreaded (bool mt)
{
  mt_override = (mt ? 1 : 0);
}

#ifdef _GL_MULTITHREADED_VIA_ELF

/* Three possible approaches come to mind for automatically determining
   whether a process is single-threaded:

     * Look at the number of entries in the /proc/self/task/ directory.
       If there is only one, the process is (currently) single-threaded.
       Drawbacks:
         - It works only on Linux.
         - It is relatively expensive (several system calls).

     * Look at the state of the PLT and GOT entries for the functions
       'pthread_create' and 'thrd_create'.  If they are both still in the
       initial state, neither of them has been called so far, and the
       process therefore is (currently) single-threaded.
       Drawbacks:
         - It does not work on modern Linux distros, because they link
           with options '-z now -z relro' ('-z relro' for security
           reasons, and '-z now' because it it required for '-z now')
           and '-z now' activates early binding instead of lazy binding
           in the PLT and GOT.
         - The code for doing this is architecture dependent (because
           the instructions in the PLT are obviously arch dependent)
           and OS dependent (because OpenBSD has a different structure
           for the PLT than the other OSes).  Additionally it requires
           inline assembly in order to get the value of the
           'pthread_create@plt' and 'thrd_create@plt' symbols.

     * Look at whether the symbols 'pthread_create' and 'thrd_create'
       occur in the PLT and GOT.  If neither of them occurs (in the
       executable and in the linked shared libraries), the process
       cannot create threads.
       Drawbacks:
         - This approach works only under the following assumptions:
           . These two entry points are the only facilities in libc
             that create threads.
           . The process will not invoke dlopen() to attach other
             shared libraries.
           . The program is not statically linked.

   Here we implement the third approach.  We mitigate the drawback
   by allowing the programmer to override the result if the assumptions
   are not met.  */

#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>

/* Old versions of <elf.h> lack these types.  */
#define Elf32_Relr Elf32_Word
#define Elf64_Relr Elf64_Xword
/* Haiku lacks this macro.  */
#ifndef PF_W
# define PF_W 0x2
#endif
/* NetBSD 9 lacks this macro.  */
#ifndef DT_GNU_HASH
# define DT_GNU_HASH 0x6ffffef5
#endif

/* Definition of __ELF_NATIVE_CLASS.  */
#ifndef __ELF_NATIVE_CLASS
# if PTRDIFF_WIDTH == 64
#  define __ELF_NATIVE_CLASS 64
# else
#  define __ELF_NATIVE_CLASS 32
# endif
#endif

/* Macros that use __ELF_NATIVE_CLASS.  */
/* Macros for decomposing an r_info field into symbol index and reloc type.
   Most platforms (except FreeBSD) lack them.  */
#if !(defined ELF_R_SYM && defined ELF_R_TYPE)
# if __ELF_NATIVE_CLASS == 32
#  define ELF_R_SYM ELF32_R_SYM
#  define ELF_R_TYPE ELF32_R_TYPE
# elif __ELF_NATIVE_CLASS == 64
#  define ELF_R_SYM ELF64_R_SYM
#  define ELF_R_TYPE ELF64_R_TYPE
# endif
#endif
/* OpenBSD lacks this macro.  */
#ifndef ElfW
# define ElfW(type) ElfW_1 (__ELF_NATIVE_CLASS, type)
# define ElfW_1(class,type) ElfW_2 (class, type)
# define ElfW_2(class,type) Elf ## class ## _ ## type
#endif

#include "thread-creators.h"

/* We inspect the GOT.  This is more robust than inspecting the PLT, because
     - When the executable is built with option '-fno-plt', there is no PLT,
       only a GOT.
     - The ELF data structures accessible from dl_iterate_phdr() don't
       actually contain a pointer to the PLT.  In order to recognize the PLT,
       we would have to look for architecture specific relocations.  */

/* Inspects the GOT of a single ELF file in memory.
   Returns 1 if a thread creator symbol is seen,
           -1 if the executable is statically linked, or
           0 otherwise.  */
static int
inspect_one_GOT (struct dl_phdr_info *info, size_t size, void *data)
{
  /* IMPORTANT: Before changing this code, make the corresponding changes
     to thread-optim-proto.c and test them.  */

  /* Skip the processing of libc.so.
     * On specific platforms, this is necessary:
         - FreeBSD 12/arm64 has lio_listio in the GOT of libc.so.
         - NetBSD 10 has timer_create in the GOT of libc.so.
         - Android has pthread_create in the GOT of libc.so.
       This does not make the process multithreaded.
     * On the other platforms, it is just an optimization, to not scan
       the (possibly many) symbols of libc.so.  */
  {
    const char *filename = info->dlpi_name;
    const char *last_slash = strrchr (filename, '/');
    if (last_slash != NULL
        && strncmp (last_slash + 1, "libc.so", 7) == 0
        && (last_slash[1 + 7] == '\0' || last_slash[1 + 7] == '.'))
      return 0;
  }

  ElfW(Addr) base_address = info->dlpi_addr;

  size_t num_headers = info->dlpi_phnum;
  const ElfW(Phdr) *headers = info->dlpi_phdr;

  const ElfW(Dyn) *dynamic_section = NULL;
  ElfW(Word) dynamic_flags _GL_ATTRIBUTE_MAYBE_UNUSED = 0;
  for (size_t h = 0; h < num_headers; h++)
    {
      const ElfW(Phdr) *header = headers + h;
      if (header->p_type == PT_DYNAMIC)
        {
          dynamic_section = (const ElfW(Dyn) *) (base_address + header->p_vaddr);
          dynamic_flags = header->p_flags;
          break;
        }
    }
  if (dynamic_section != NULL)
    {
      ElfW(Addr) maybe_base_address;
#if __GLIBC__ >= 2 && !(defined __mips__ || defined __riscv)
      /* linux-vdso.so.1 is weird.  */
      if (dynamic_flags & PF_W)
        maybe_base_address = 0;
      else
#endif
        maybe_base_address = base_address;

      /* Get the string table.  */
      const char *strtab = NULL;
      size_t strsz = 0;
      for (const ElfW(Dyn) *dynamic_section_entry = dynamic_section;
           dynamic_section_entry->d_tag != DT_NULL;
           dynamic_section_entry++)
        {
          if (dynamic_section_entry->d_tag == DT_STRTAB)
            strtab = (const char *) (maybe_base_address + dynamic_section_entry->d_un.d_ptr);
          if (dynamic_section_entry->d_tag == DT_STRSZ)
            strsz = dynamic_section_entry->d_un.d_val;
        }

      /* Get the symbol table.  */
      const ElfW(Sym) *symtab = NULL;
      size_t symentsz = 0;
      const unsigned int *hash = NULL;
      const unsigned int *gnu_hash = NULL;
      for (const ElfW(Dyn) *dynamic_section_entry = dynamic_section;
           dynamic_section_entry->d_tag != DT_NULL;
           dynamic_section_entry++)
        {
          if (dynamic_section_entry->d_tag == DT_SYMTAB)
            symtab = (const ElfW(Sym) *) (maybe_base_address + dynamic_section_entry->d_un.d_ptr);
          if (dynamic_section_entry->d_tag == DT_SYMENT)
            symentsz = dynamic_section_entry->d_un.d_val;
          if (dynamic_section_entry->d_tag == DT_HASH)
            hash = (const unsigned int *) (maybe_base_address + dynamic_section_entry->d_un.d_ptr);
          if (dynamic_section_entry->d_tag == DT_GNU_HASH)
            gnu_hash = (const unsigned int *) (maybe_base_address + dynamic_section_entry->d_un.d_ptr);
        }
      if (!(symentsz == sizeof (ElfW(Sym))))
        /* Invalid symentsz.
           Should not happen with properly formed ELF files.  */
        abort ();
      size_t num_symtab_entries = (size_t)-1;
      if (hash != NULL)
        {
          /* The DT_HASH pointer points to
               struct elf_hash_table
               {
                 uint32_t nbucket;
                 uint32_t nchain;
                 uint32_t bucket[nbucket];
                 uint32_t chain[nchain];
               }
             where nchain is the number of entries in the symbol table.
             See <https://flapenguin.me/elf-dt-hash>.  */
          num_symtab_entries = hash[1];
        }
      if (gnu_hash != NULL)
        {
          /* The DT_GNU_HASH pointer points to
               struct gnu_hash_table
               {
                 uint32_t nbuckets;
                 uint32_t symoffset;
                 uint32_t bloom_size;
                 uint32_t bloom_shift;
                 ElfW(Word) bloom[bloom_size];
                 uint32_t buckets[nbuckets];
                 uint32_t chain[];
               }
             See <https://flapenguin.me/elf-dt-gnu-hash> and
             binutils/bfd/elflink.c:bfd_elf_size_dynsym_hash_dynstr().  */
          unsigned int nbuckets = gnu_hash[0];
          unsigned int symoffset = gnu_hash[1];
          unsigned int bloom_size = gnu_hash[2];
          const unsigned int *buckets_start = &gnu_hash[4 + bloom_size * (__ELF_NATIVE_CLASS / 32)];
          size_t gnu_num_symtab_entries;
          if (nbuckets == 1 && symoffset == 1 && bloom_size == 1 && gnu_hash[3] == 0
              && buckets_start[0] == 0)
            gnu_num_symtab_entries = 0;
          else
            {
              const unsigned int *chain_start = &buckets_start[nbuckets];
              const unsigned int *chain_p = chain_start;
              for (unsigned int bucket = 0; bucket < nbuckets; bucket++)
                if (buckets_start[bucket] != 0)
                  {
                    while ((*chain_p & 1) == 0)
                      chain_p++;
                    chain_p++;
                  }
              gnu_num_symtab_entries = symoffset + (chain_p - chain_start);
            }
          if (gnu_num_symtab_entries != 0)
            num_symtab_entries = gnu_num_symtab_entries;
        }

# if defined __mips__

      /* MIPS does not have the usual GOT structure.
         Instead, inspect the symbols in the .dynsym section.  This happens
         to be the same set of symbols as the symbols in the symtab marked as
         UNDEF.  This is a somewhat larger set of symbols, but still good
         enough.  */
      for (size_t symbol_index = 1; symbol_index < num_symtab_entries; symbol_index++)
        {
          const ElfW(Sym) *symtab_entry = symtab + symbol_index;
          if (symtab_entry->st_shndx == SHN_UNDEF)
            {
              ElfW(Word) symbol_name_offset = symtab_entry->st_name;
              const char *symbol_name = strtab + symbol_name_offset;
              if (thread_creators_lookup (symbol_name, strlen (symbol_name)) != NULL)
                /* Found a jump relocation to a thread creator symbol.  */
                return 1;
            }
        }

#else

      int jump_relocation_type = 0;
      const void *jump_relocations = NULL;
      size_t jump_relocations_size = 0;
      for (const ElfW(Dyn) *dynamic_section_entry = dynamic_section;
           dynamic_section_entry->d_tag != DT_NULL;
           dynamic_section_entry++)
        {
          if (dynamic_section_entry->d_tag == DT_PLTREL)
            {
              /* The value should be DT_REL or DT_RELA.  */
              jump_relocation_type = dynamic_section_entry->d_un.d_val;
            }
          else if (dynamic_section_entry->d_tag == DT_PLTRELSZ)
            {
              jump_relocations_size = dynamic_section_entry->d_un.d_val;
            }
          else if (dynamic_section_entry->d_tag == DT_JMPREL)
            {
# if __GLIBC__ >= 2 && !defined __riscv
              jump_relocations = (const void *) dynamic_section_entry->d_un.d_ptr;
# else /* glibc/riscv, musl libc, FreeBSD, NetBSD, OpenBSD, Solaris, Haiku, Android */
              jump_relocations = (const void *) (base_address + dynamic_section_entry->d_un.d_ptr);
# endif
            }
          /* The entry with tag DT_PLTGOT contains a pointer to the GOT
             in memory.  But we don't need it actually.  */
        }
      if (jump_relocations != NULL)
        {
          if (!(jump_relocation_type == DT_REL || jump_relocation_type == DT_RELA))
            /* No valid jump_relocation_type.
               Should not happen with properly formed ELF files.  */
            abort ();

          size_t num_jump_relocations =
            jump_relocations_size
            / (jump_relocation_type == DT_RELA ? sizeof (ElfW(Rela)) : sizeof (ElfW(Rel)));

          for (size_t j = 0; j < num_jump_relocations; j++)
            {
              ElfW(Addr) got_element _GL_ATTRIBUTE_MAYBE_UNUSED;
              ElfW(Relr) r_info;
              if (jump_relocation_type == DT_RELA)
                {
                  const ElfW(Rela) *jump_reloc = (const ElfW(Rela) *) jump_relocations + j;
                  got_element = base_address + jump_reloc->r_offset;
                  r_info = jump_reloc->r_info;
                }
              else /* jump_relocation_type == DT_REL */
                {
                  const ElfW(Rel) *jump_reloc = (const ElfW(Rel) *) jump_relocations + j;
                  got_element = base_address + jump_reloc->r_offset;
                  r_info = jump_reloc->r_info;
                }
              unsigned int symbol_index = ELF_R_SYM (r_info);
              unsigned int reloc_type = ELF_R_TYPE (r_info);
              (void) reloc_type;
              /* We can ignore relocations not connected to a symbol.  */
              if (symbol_index != 0)
                {
                  if (!(symbol_index < num_symtab_entries))
                    /* Out-of-range reference to the symtab.
                       Should not happen with properly formed ELF files.  */
                    abort ();
                  const ElfW(Sym) *symtab_entry = symtab + symbol_index;
                  ElfW(Word) symbol_name_offset = symtab_entry->st_name;
                  if (!(symbol_name_offset < strsz))
                    /* Out-of-range reference to the strtab.
                       Should not happen with properly formed ELF files.  */
                    abort ();
                  const char *symbol_name = strtab + symbol_name_offset;
                  if (thread_creators_lookup (symbol_name, strlen (symbol_name)) != NULL)
                    /* Found a jump relocation to a thread creator symbol.  */
                    return 1;
                }
            }
        }

#endif
    }
  else
    {
      if (info->dlpi_name[0] == '\0')
        /* The executable is statically linked.  */
        return -1;
    }

  return 0;
}

/* Returns true if the process is possibly multithreaded throughout its
   lifetime (assuming the process will not invoke dlopen() to attach other
   shared libraries).  */
static bool
is_multithreaded_uncached (void)
{
  int result = dl_iterate_phdr (inspect_one_GOT, NULL);
  /* result is 1 if a thread creator symbol is seen in some of the GOTs,
               -1 if the executable is statically linked, or
               0 otherwise.  */
  return result != 0;
}

bool
gl_multithreaded (void)
{
  /* Consider the override.  */
  if (mt_override >= 0)
    return mt_override;
  else
    {
      /* Cache the result from is_multithreaded_uncached.  */
      static int volatile cached_result = -1;

      if (cached_result < 0)
        cached_result = is_multithreaded_uncached ();

      return cached_result;
    }
}

#endif /* _GL_MULTITHREADED_VIA_ELF */
