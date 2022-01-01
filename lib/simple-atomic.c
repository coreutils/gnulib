/* Simple atomic operations for multithreading.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#include <config.h>

/* Specification.  */
#include "simple-atomic.h"

#if 0x590 <= __SUNPRO_C && __STDC__
# define asm __asm
#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Native Windows.  */

# include <windows.h>

void
memory_barrier (void)
{
  /* MemoryBarrier
     <https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-memorybarrier>  */
  MemoryBarrier ();
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
  /* InterlockedCompareExchange
     <https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedcompareexchange>  */
  return InterlockedCompareExchange ((LONG volatile *) vp,
                                     (LONG) newval, (LONG) cmp);
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
  /* InterlockedCompareExchangePointer
     <https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedcompareexchangepointer>  */
  return InterlockedCompareExchangePointer ((void * volatile *) vp,
                                            (void *) newval, (void *) cmp);
}

#elif HAVE_PTHREAD_H
/* Some other platform that supports multi-threading.

   We don't use the C11 <stdatomic.h> (available in GCC >= 4.9) because it would
   require to link with -latomic.  */

# if (((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) \
       && !defined __sparc__) \
      || __clang_major__ >= 3) \
     && !defined __ibmxl__
/* Use GCC built-ins (available in GCC >= 4.1, except on SPARC, and
   clang >= 3.0).
   Documentation:
   <https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html>  */

void
memory_barrier (void)
{
  __sync_synchronize ();
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
  return __sync_val_compare_and_swap (vp, cmp, newval);
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
  return __sync_val_compare_and_swap (vp, cmp, newval);
}

# elif defined _AIX
/* AIX */
/* For older versions of GCC or xlc, use inline assembly.
   __compare_and_swap and __compare_and_swaplp are not sufficient here.  */

void
memory_barrier (void)
{
  asm volatile ("sync");
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
  asm volatile ("sync");

  unsigned int oldval;
  asm volatile (
#  if defined __GNUC__ || defined __clang__
                "1: lwarx %0,0,%1\n"
                  " cmpw 0,%0,%2\n"
                  " bne 0,2f\n"
                  " stwcx. %3,0,%1\n"
                  " bne 0,1b\n"
                "2:"
#  else /* another label syntax */
                ".L01: lwarx %0,0,%1\n"
                     " cmpw 0,%0,%2\n"
                     " bne 0,.L02\n"
                     " stwcx. %3,0,%1\n"
                     " bne 0,.L01\n"
                ".L02:"
#  endif
                : "=&r" (oldval)
                : "r" (vp), "r" (cmp), "r" (newval)
                : "cr0");

  asm volatile ("isync");
  return oldval;
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
  asm volatile ("sync");

  uintptr_t oldval;
  asm volatile (
#  if defined __GNUC__ || defined __clang__
#   if defined __powerpc64__ || defined __LP64__
                "1: ldarx %0,0,%1\n"
                  " cmpd 0,%0,%2\n"
                  " bne 0,2f\n"
                  " stdcx. %3,0,%1\n"
                  " bne 0,1b\n"
                "2:"
#   else
                "1: lwarx %0,0,%1\n"
                  " cmpw 0,%0,%2\n"
                  " bne 0,2f\n"
                  " stwcx. %3,0,%1\n"
                  " bne 0,1b\n"
                "2:"
#   endif
#  else /* another label syntax */
#   if defined __powerpc64__ || defined __LP64__
                ".L01: ldarx %0,0,%1\n"
                     " cmpd 0,%0,%2\n"
                     " bne 0,.L02\n"
                     " stdcx. %3,0,%1\n"
                     " bne 0,.L01\n"
                ".L02:"
#   else
                ".L01: lwarx %0,0,%1\n"
                     " cmpw 0,%0,%2\n"
                     " bne 0,.L02\n"
                     " stwcx. %3,0,%1\n"
                     " bne 0,.L01\n"
                ".L02:"
#   endif
#  endif
                : "=&r" (oldval)
                : "r" (vp), "r" (cmp), "r" (newval)
                : "cr0");

  asm volatile ("isync");
  return oldval;
}

# elif ((defined __GNUC__ || defined __clang__ || defined __SUNPRO_C) && (defined __sparc || defined __i386 || defined __x86_64__)) || (defined __TINYC__ && (defined __i386 || defined __x86_64__))
/* For older versions of GCC or clang, use inline assembly.
   GCC, clang, and the Oracle Studio C 12 compiler understand GCC's extended
   asm syntax, but the plain Oracle Studio C 11 compiler understands only
   simple asm.  */

void
memory_barrier (void)
{
#  if defined __GNUC__ || defined __clang__ || __SUNPRO_C >= 0x590 || defined __TINYC__
#   if defined __i386 || defined __x86_64__
#    if defined __TINYC__ && defined __i386
  /* Cannot use the SSE instruction "mfence" with this compiler.  */
  asm volatile ("lock orl $0,(%esp)");
#    else
  asm volatile ("mfence");
#    endif
#   endif
#   if defined __sparc
  asm volatile ("membar 2");
#   endif
#  else
#   if defined __i386 || defined __x86_64__
  asm ("mfence");
#   endif
#   if defined __sparc
  asm ("membar 2");
#   endif
#  endif
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
#  if defined __GNUC__ || defined __clang__ || __SUNPRO_C >= 0x590 || defined __TINYC__
  unsigned int oldval;
#   if defined __i386 || defined __x86_64__
  asm volatile (" lock\n cmpxchgl %3,(%1)"
                : "=a" (oldval) : "r" (vp), "a" (cmp), "r" (newval) : "memory");
#   endif
#   if defined __sparc
  asm volatile (" cas [%1],%2,%3\n"
                " mov %3,%0"
                : "=r" (oldval) : "r" (vp), "r" (cmp), "r" (newval) : "memory");
#   endif
  return oldval;
#  else /* __SUNPRO_C */
#   if defined __x86_64__
  asm (" movl %esi,%eax\n"
       " lock\n cmpxchgl %edx,(%rdi)");
#   elif defined __i386
  asm (" movl 16(%ebp),%ecx\n"
       " movl 12(%ebp),%eax\n"
       " movl 8(%ebp),%edx\n"
       " lock\n cmpxchgl %ecx,(%edx)");
#   endif
#   if defined __sparc
  asm (" cas [%i0],%i1,%i2\n"
       " mov %i2,%i0");
#   endif
#  endif
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
#  if defined __GNUC__ || defined __clang__ || __SUNPRO_C >= 0x590 || defined __TINYC__
  uintptr_t oldval;
#   if defined __x86_64__
  asm volatile (" lock\n cmpxchgq %3,(%1)"
                : "=a" (oldval) : "r" (vp), "a" (cmp), "r" (newval) : "memory");
#   elif defined __i386
  asm volatile (" lock\n cmpxchgl %3,(%1)"
                : "=a" (oldval) : "r" (vp), "a" (cmp), "r" (newval) : "memory");
#   endif
#   if defined __sparc && (defined __sparcv9 || defined __arch64__)
  asm volatile (" casx [%1],%2,%3\n"
                " mov %3,%0"
                : "=r" (oldval) : "r" (vp), "r" (cmp), "r" (newval) : "memory");
#   elif defined __sparc
  asm volatile (" cas [%1],%2,%3\n"
                " mov %3,%0"
                : "=r" (oldval) : "r" (vp), "r" (cmp), "r" (newval) : "memory");
#   endif
  return oldval;
#  else /* __SUNPRO_C */
#   if defined __x86_64__
  asm (" movq %rsi,%rax\n"
       " lock\n cmpxchgq %rdx,(%rdi)");
#   elif defined __i386
  asm (" movl 16(%ebp),%ecx\n"
       " movl 12(%ebp),%eax\n"
       " movl 8(%ebp),%edx\n"
       " lock\n cmpxchgl %ecx,(%edx)");
#   endif
#   if defined __sparc && (defined __sparcv9 || defined __arch64__)
  asm (" casx [%i0],%i1,%i2\n"
       " mov %i2,%i0");
#   elif defined __sparc
  asm (" cas [%i0],%i1,%i2\n"
       " mov %i2,%i0");
#   endif
#  endif
}

# else
/* Fallback code.  It has some race conditions.  The unit test will fail.  */

void
memory_barrier (void)
{
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
  unsigned int oldval = *vp;
  if (oldval == cmp)
    *vp = newval;
  return oldval;
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
  uintptr_t oldval = *vp;
  if (oldval == cmp)
    *vp = newval;
  return oldval;
}

# endif

#else
/* A platform that does not support multi-threading.  */

void
memory_barrier (void)
{
}

unsigned int
atomic_compare_and_swap (unsigned int volatile *vp,
                         unsigned int cmp,
                         unsigned int newval)
{
  unsigned int oldval = *vp;
  if (oldval == cmp)
    *vp = newval;
  return oldval;
}

uintptr_t
atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                             uintptr_t cmp,
                             uintptr_t newval)
{
  uintptr_t oldval = *vp;
  if (oldval == cmp)
    *vp = newval;
  return oldval;
}

#endif
