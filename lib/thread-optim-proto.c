/* Prints the PLT/GOT or .dynsym symbols of all attached shared objects.

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

#define _GNU_SOURCE 1

#include <link.h>
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
/* Many older platforms lack the definitions of these relocations.  */
#ifndef R_X86_64_JUMP_SLOT
# define R_X86_64_JUMP_SLOT 7
#endif
#ifndef R_X86_64_IRELATIVE
# define R_X86_64_IRELATIVE 37
#endif
#ifndef R_386_JMP_SLOT
# define R_386_JMP_SLOT 7
#endif
#ifndef R_AARCH64_JUMP_SLOT
# define R_AARCH64_JUMP_SLOT 1026
#endif
#ifndef R_ARM_JUMP_SLOT
# define R_ARM_JUMP_SLOT 22
#endif
#ifndef R_ALPHA_JMP_SLOT
# define R_ALPHA_JMP_SLOT 26
#endif
#ifndef R_PARISC_IPLT
# define R_PARISC_IPLT 129
#endif
#ifndef R_68K_JMP_SLOT
# define R_68K_JMP_SLOT 21
#endif
#ifndef R_LARCH_JUMP_SLOT
# define R_LARCH_JUMP_SLOT 5
#endif
#ifndef R_PPC_JMP_SLOT
# define R_PPC_JMP_SLOT 21
#endif
#ifndef R_RISCV_JUMP_SLOT
# define R_RISCV_JUMP_SLOT 5
#endif
#ifndef R_390_JMP_SLOT
# define R_390_JMP_SLOT 11
#endif
#ifndef R_SH_JMP_SLOT
# define R_SH_JMP_SLOT 164
#endif
#ifndef R_SPARC_JMP_SLOT
# define R_SPARC_JMP_SLOT 21
#endif

/* Definition of __ELF_NATIVE_CLASS.  */
#ifndef __ELF_NATIVE_CLASS
/* We can't rely on PTRDIFF_WIDTH from <stdint.h> here.  */
# if defined _LP64 || defined __LP64__ \
     || ((defined __x86_64__ || defined _M_X64) && !(defined __ILP32__ || defined _ILP32)) \
     || defined __aarch64__ /* arm64 */ \
     || defined __alpha \
     || (defined __ia64__ && !defined _ILP32) \
     || (defined _MIPS_SZLONG && (_MIPS_SZLONG == 64)) /* mips64 */ \
     || defined __loongarch__ \
     || defined __powerpc64__ \
     || (defined __riscv && __riscv_xlen == 64) /* riscv64 */ \
     || defined __s390x__ \
     || (defined __sparcv9 || defined __arch64__) /* sparc64 */
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

static int
inspect_one_GOT (struct dl_phdr_info *info, size_t size, void *data)
{
  if (info->dlpi_name[0] == '\0')
    printf ("executable: ");
  else
    printf ("library %s: ", info->dlpi_name);

  printf ("size = %zu\n", size);

  ElfW(Addr) base_address = info->dlpi_addr;
  printf ("base_address = 0x%zx\n", base_address);

  size_t num_headers = info->dlpi_phnum;
  const ElfW(Phdr) *headers = info->dlpi_phdr;
  printf ("num_headers = %zu\n", num_headers);

  const ElfW(Dyn) *dynamic_section = NULL;
  ElfW(Word) dynamic_flags = 0;
  for (size_t h = 0; h < num_headers; h++)
    {
      const ElfW(Phdr) *header = headers + h;
      if (header->p_type == PT_DYNAMIC)
        {
#if 0
          printf ("Header %zu:\n", h);
          printf ("  type = %zu\n", (size_t) header->p_type);
          printf ("  flags = 0x%zx\n", (size_t) header->p_flags);
          printf ("  offset = 0x%zx\n", (size_t) header->p_offset);
          printf ("  vaddr = 0x%zx\n", (size_t) header->p_vaddr);
          printf ("  paddr = 0x%zx\n", (size_t) header->p_paddr);
          printf ("  filesz = 0x%zx\n", (size_t) header->p_filesz);
          printf ("  memsz = 0x%zx\n", (size_t) header->p_memsz);
          printf ("  align = 0x%zx\n", (size_t) header->p_align);
#endif
          dynamic_section = (const ElfW(Dyn) *) (base_address + header->p_vaddr);
          dynamic_flags = header->p_flags;
          break;
        }
    }
  printf ("Dynamic section = 0x%zx\n", (ElfW(Addr)) dynamic_section);
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
      printf ("strtab = 0x%zx, total size strsz = %zu\n", (ElfW(Addr)) strtab, strsz);
      printf ("symtab = 0x%zx, each element of size symentsz = %zu\n", (ElfW(Addr)) symtab, symentsz);
      /* Check the symentsz.  */
      if (!(symentsz == sizeof (ElfW(Sym))))
        /* Invalid symentsz.
           Should not happen with properly formed ELF files.  */
        abort ();
      /* How many symbols in symtab?
         The old method uses DT_HASH, newer binaries use DT_GNU_HASH.  */
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
          printf ("num_symtab_entries according to HASH = %zu\n", num_symtab_entries);
          /* On Linux/s390x this value is too small.  The one from GNU_HASH is correct.  */
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
          printf ("num_symtab_entries according to GNU_HASH = %zu\n", gnu_num_symtab_entries);
          if (gnu_num_symtab_entries != 0)
            num_symtab_entries = gnu_num_symtab_entries;
        }

      /* List the symbols in the .dynsym section.
         $ readelf -a -X a.out  */
      if (num_symtab_entries > 0 && num_symtab_entries < (size_t)-1)
        {
          printf ("Symbols in the .dynsym section:\n");
          for (size_t symbol_index = 1; symbol_index < num_symtab_entries; symbol_index++)
            {
              const ElfW(Sym) *symtab_entry = symtab + symbol_index;
              if (symtab_entry->st_shndx == SHN_UNDEF)
                {
                  ElfW(Word) symbol_name_offset = symtab_entry->st_name;
                  const char *symbol_name = strtab + symbol_name_offset;
                  printf ("Symtab[%zu]: %s\n", symbol_index, symbol_name);
                }
            }
        }

#if defined __mips__ /* MIPS does not have the usual GOT structure */

#if 0 /* works but not useful */

      const void *got_entries = NULL;
      size_t num_got_entries = 0;
      for (const ElfW(Dyn) *dynamic_section_entry = dynamic_section;
           dynamic_section_entry->d_tag != DT_NULL;
           dynamic_section_entry++)
        {
          if (dynamic_section_entry->d_tag == DT_PLTGOT)
            {
              printf ("PLTGOT value: 0x%zx\n", dynamic_section_entry->d_un.d_ptr);
              got_entries = (const void *) dynamic_section_entry->d_un.d_ptr;
            }
          if (dynamic_section_entry->d_tag == DT_MIPS_LOCAL_GOTNO)
            {
              printf ("DT_MIPS_LOCAL_GOTNO value: 0x%zx\n", dynamic_section_entry->d_un.d_val);
              num_got_entries = dynamic_section_entry->d_un.d_val;
            }
        }

#endif

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
              printf ("PLTREL value: 0x%zx", dynamic_section_entry->d_un.d_val);
#if defined __x86_64__ || defined __aarch64__ || defined __alpha || defined __hppa || defined __m68k__ || defined _ARCH_PPC || defined __riscv || defined __s390__ || defined __sh__ || defined __sparc
              if (dynamic_section_entry->d_un.d_val == DT_RELA)
                printf (" = DT_RELA");
#elif defined __i386__ || defined __arm__
              if (dynamic_section_entry->d_un.d_val == DT_REL)
                printf (" = DT_REL");
#endif
              jump_relocation_type = dynamic_section_entry->d_un.d_val;
              printf ("\n");
            }
          else if (dynamic_section_entry->d_tag == DT_PLTRELSZ)
            {
              printf ("PLTRELSZ value: 0x%zx\n", dynamic_section_entry->d_un.d_val);
              jump_relocations_size = dynamic_section_entry->d_un.d_val;
            }
          else if (dynamic_section_entry->d_tag == DT_JMPREL)
            {
              printf ("JMPREL value: 0x%zx\n", dynamic_section_entry->d_un.d_ptr);
#if __GLIBC__ >= 2 && !defined __riscv
              jump_relocations = (const void *) dynamic_section_entry->d_un.d_ptr;
#else /* glibc/riscv, musl libc, FreeBSD, NetBSD, OpenBSD, Solaris, Haiku, Android */
              jump_relocations = (const void *) (base_address + dynamic_section_entry->d_un.d_ptr);
#endif
            }
          else if (dynamic_section_entry->d_tag == DT_PLTGOT)
            {
              /* This points to the GOT. It contains the real function addresses,
                 but no symbolic information.  */
              printf ("PLTGOT value: 0x%zx\n", dynamic_section_entry->d_un.d_ptr);
            }
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
              ElfW(Addr) got_element;
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
                  printf ("Jump reloc: address = 0x%zx, reloc_type = 0x%x",
                          got_element, reloc_type);
#if defined __x86_64__
                  if (reloc_type == R_X86_64_JUMP_SLOT)
                    printf (" = R_X86_64_JUMP_SLOT");
                  else if (reloc_type == R_X86_64_IRELATIVE)
                    printf (" = R_X86_64_IRELATIVE");
#elif defined __i386__
                  if (reloc_type == R_386_JMP_SLOT)
                    printf (" = R_386_JMP_SLOT");
#elif defined __aarch64__
                  if (reloc_type == R_AARCH64_JUMP_SLOT)
                    printf (" = R_AARCH64_JUMP_SLOT");
#elif defined __arm__
                  if (reloc_type == R_ARM_JUMP_SLOT)
                    printf (" = R_ARM_JUMP_SLOT");
#elif defined __alpha
                  if (reloc_type == R_ALPHA_JMP_SLOT)
                    printf (" = R_ALPHA_JMP_SLOT");
#elif defined __hppa
                  if (reloc_type == R_PARISC_IPLT)
                    printf (" = R_PARISC_IPLT");
#elif defined __m68k__
                  if (reloc_type == R_68K_JMP_SLOT)
                    printf (" = R_68K_JMP_SLOT");
#elif defined __loongarch__
                  if (reloc_type == R_LARCH_JUMP_SLOT)
                    printf (" = R_LARCH_JUMP_SLOT");
#elif defined _ARCH_PPC
                  if (reloc_type == R_PPC_JMP_SLOT)
                    printf (" = R_PPC_JMP_SLOT");
#elif defined __riscv
                  if (reloc_type == R_RISCV_JUMP_SLOT)
                    printf (" = R_RISCV_JUMP_SLOT");
#elif defined __s390__
                  if (reloc_type == R_390_JMP_SLOT)
                    printf (" = R_390_JMP_SLOT");
#elif defined __sh__
                  if (reloc_type == R_SH_JMP_SLOT)
                    printf (" = R_SH_JMP_SLOT");
#elif defined __sparc
                  if (reloc_type == R_SPARC_JMP_SLOT)
                    printf (" = R_SPARC_JMP_SLOT");
#endif
                  printf (", symbol_index = 0x%x = %s",
                          symbol_index, symbol_name);
                  printf ("\n");
                }
            }
        }

#endif
    }
  printf ("\n");

  return 0;
}

int
main ()
{
  /* For debugging: Prevent fully buffered mode of stdout.  */
  setvbuf (stdout, NULL, _IOLBF, 0);

  dl_iterate_phdr (inspect_one_GOT, NULL);

  return 0;
}
