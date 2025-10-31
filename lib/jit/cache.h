/* JIT compiler - Flushing the instruction cache.

   Copyright (C) 1995-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <stdint.h>
#if ENABLE_VALGRIND_SUPPORT
# include <valgrind/valgrind.h>
#endif
#if defined _WIN32 && !defined __CYGWIN__
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif
#if defined __APPLE__ && defined __MACH__
# include <libkern/OSCacheControl.h>
#endif
#if defined _AIX
# include <sys/cache.h>
#endif
#if defined __sun
# include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Clears the instruction cache for addresses
   start <= address < end.
   We need this because some CPUs have separate data cache and instruction
   cache. The freshly built trampoline is visible to the data cache, but
   maybe not to the instruction cache. This is hairy.  */
static inline void
clear_cache (void *start, void *end)
{
#if ENABLE_VALGRIND_SUPPORT
  /* Documentation:
     <https://valgrind.org/docs/manual/manual-core-adv.html#manual-core-adv.clientreq>  */
  VALGRIND_DISCARD_TRANSLATIONS (start, (char *) end - (char *) start);
#endif
#if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)
  /* On this architecture, data cache and instruction cache are not separate.
     Therefore, nothing to do.
     For details, see
     <https://stackoverflow.com/questions/10989403/how-is-x86-instruction-cache-synchronized>  */

/* Use the operating system provided function, when available.  */
#elif defined _WIN32 && !defined __CYGWIN__
  /* Native Windows.
     FlushInstructionCache
     <https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-flushinstructioncache>  */
  HANDLE process = GetCurrentProcess ();
  while (!FlushInstructionCache (process, start, (char *) end - (char *) start))
    ;
#elif defined __APPLE__ && defined __MACH__
  /* macOS  */
  sys_icache_invalidate (start, (char *) end - (char *) start);
#elif defined _AIX
  /* AIX.  */
  _sync_cache_range (start, (char *) end - (char *) start);
#elif defined __sun
  /* Solaris.  */
  extern void sync_instruction_memory (char *, size_t);
  sync_instruction_memory (start, (char *) end - (char *) start);

/* No operating system provided function. Dispatch according to the CPU.  */
#elif (defined __GNUC__ || defined __clang__) && defined _ARCH_PPC
  /* XXX Is this enough, or do we also need the 'clf' instruction?  */
  uintptr_t addr = (uintptr_t) start & ~(intptr_t)3;
  uintptr_t end_addr = (uintptr_t) end;
  do
    {
      asm volatile ("icbi 0,%0; dcbf 0,%0" : : "r" (addr));
      addr += 4;
    }
  while (addr < end_addr);
  asm volatile ("sync; isync");
#elif (defined __GNUC__ || defined __clang__) && defined __sparc
  /* Use inline assembly.  */
  /* The 'flush' instruction was earlier called 'iflush'.  */
  uintptr_t addr = (uintptr_t) start & ~(intptr_t)7;
  uintptr_t end_addr = (uintptr_t) end;
  do
    {
      asm volatile ("flush %0+0" : : "r" (addr));
      addr += 8;
    }
  while (addr < end_addr);
#elif (defined __GNUC__ || defined __clang__) && defined __hppa
  /* Use inline assembly.  */
  /* The PA-RISC 1.1 Architecture and Instruction Set Reference Manual says:
     "A cache line can be 16, 32, or 64 bytes in length."  */
  /* XXX Is this good enough, or do we need the space register business
     like in gcc/gcc/config/pa/pa.md and libffcall/trampoline/cache-hppa.c?  */
  intptr_t cache_line_size = 16;
  uintptr_t addr = (uintptr_t) start & ~cache_line_size;
  uintptr_t end_addr = (uintptr_t) end;
  do
    {
      asm volatile ("fdc 0(0,%0)"
             "\n\t" "sync"
             "\n\t" "fic 0(0,%0)"
             "\n\t" "sync" : : "r" (addr));
      addr += cache_line_size;
    }
  while (addr < end_addr);
  asm volatile ("nop"
         "\n\t" "nop"
         "\n\t" "nop"
         "\n\t" "nop"
         "\n\t" "nop"
         "\n\t" "nop");
#elif (defined __GNUC__ || defined __clang__) && defined __ia64
  /* Use inline assembly.  */
  /* The Intel IA-64 Architecture Software Developer's Manual volume 3 says:
     "The line size affected is at least 32 bytes."  */
  intptr_t cache_line_size = 32;
  uintptr_t addr = (uintptr_t) start & ~cache_line_size;
  uintptr_t end_addr = (uintptr_t) end;
  do
    {
      /* Flush a cache line.  */
      asm volatile ("fc %0" : : "r" (addr));
      addr += cache_line_size;
    }
  while (addr < end_addr);
  /* Ensure the preceding 'fc' instructions become effective in the local
     processor and all remote processors.  */
  asm volatile ("sync.i");
  /* Ensure the preceding 'sync.i' instruction becomes effective in the
     local processor's instruction cache.  */
  asm volatile ("srlz.i");
#elif (defined __GNUC__ || defined __clang__) && defined __m68k__ && defined __linux__
  /* Use inline assembly to call the 'cacheflush' system call.
     sys_cacheflush (addr, scope, cache, len)
                      d1     d2     d3    d4
   */
  register uintptr_t addr __asm__ ("%d1") = (uintptr_t) start;
  register uintptr_t len __asm__ ("%d4") = (uintptr_t) end - addr;
  __asm__ __volatile__ (
           "move%.l %#123,%/d0" /* __NR_cacheflush */
    "\n\t" "move%.l %#1,%/d2"   /* FLUSH_SCOPE_LINE */
    "\n\t" "move%.l %#3,%/d3"   /* FLUSH_CACHE_BOTH */
    "\n\t" "trap %#0"
    :
    : "d" (addr), "d" (len)
    : "%d0", "%d2", "%d3"
    );
#elif (__GNUC__ + (__GNUC_MINOR__ >= 3) > 4 && !defined __clang__) \
      || ((__clang_major__ + (__clang_minor__ >= 4) > 3) \
          && (defined __aarch64__ /* arm64 */ || defined __arm__))
  /* GCC >= 4.3 has a GCC built-in.
     <https://gcc.gnu.org/onlinedocs/gcc-4.3.6/gcc/Other-Builtins.html>
     But it's sometimes not correctly implemented.
     clang >= 3.4 has it as well, at least on ARM and ARM64.  */
  /* On ARM, cache flushing can only be done through a system call.
     GCC implements it for Linux with EABI, through an "swi 0" with code
     0xf0002.  For other systems, it may be an "swi 0x9f0002",
     an "swi 0xf00000", or similar.  */
  /* On ARM64, cache flushing is done through special instructions,
     and the length of the cache lines must be determined at runtime.
     See gcc/libgcc/config/aarch64/sync-cache.c.  */
  __builtin___clear_cache (start, end);
#elif HAVE___CLEAR_CACHE
  /* Older versions of GCC have this libgcc function, but only on some
     platforms.  */
  extern void __clear_cache (char *, char *);
  __clear_cache (start, end);
#else
# error "Don't know how to implement clear_cache on this platform."
#endif
}


#ifdef __cplusplus
}
#endif
