/* Functions for saving the floating-point exception status flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.

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

/* Based on glibc/math/fetestexceptflag.c
   together with glibc/sysdeps/<cpu>/{fpu_control.h,fenv_private.h,fenv_libc.h}.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#if (defined __x86_64__ || defined _M_X64) || (defined __i386 || defined _M_IX86)

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __aarch64__ /* arm64 */

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long flags = (unsigned long) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __arm__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __alpha

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long flags = (unsigned long) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __hppa

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __ia64__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long flags = (unsigned long) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __m68k__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __mips__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __loongarch__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined _ARCH_PPC

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __riscv

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __s390__ || defined __s390x__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __sh__

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned int flags = (unsigned int) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#elif defined __sparc

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  unsigned long flags = (unsigned long) *saved_flags;
  return flags & FE_ALL_EXCEPT & exceptions;
}

#else

# if defined __GNUC__ || defined __clang__
#   warning "Unknown CPU / architecture. Please report your platform and compiler to <bug-gnulib@gnu.org>."
#  endif
# define NEED_FALLBACK 1

#endif

#if NEED_FALLBACK

/* A dummy fallback.  */

int
fetestexceptflag (fexcept_t const *saved_flags, int exceptions)
{
  return 0;
}

#endif
