/* Memory allocation with expensive empty allocations.
   Copyright (C) 2003, 2008, 2010-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2003,
   based on prior work by Jim Meyering.

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

#ifndef _EEALLOC_H
#define _EEALLOC_H

/* malloc (0) and realloc (NULL, 0) can return NULL even when memory
   is available; see ISO C 23 sections 7.24.3.

   When P is non-null realloc (P, 0) is worse, in that C23 says the
   behavior is undefined whereas POSIX.1-2024 (which extends C17) says
   that realloc (P, 0) when successful behaves like either (free (P),
   errno = EINVAL, NULL), or like (free (P), malloc (1)) returning non-null
   so long as you do not dereference the non-null pointer;
   and glibc does not conform to POSIX as it behaves like (free (P), NULL).
   There are similar issues with reallocarray.

   This behavior is sometimes unwelcome, as it entails extra checking
   to avoid confusing a zero-sized allocation with memory exhaustion,
   and to avoid undefined behavior in C23.

   This file provides malloc and realloc workalikes that consistently
   treat zero sizes as requests for zero-sized allocations instead of
   for null pointers.  However, it does not provide workalikes for
   related functions like aligned_alloc, calloc, and reallocarray.  */

/* This file uses _GL_INLINE_HEADER_BEGIN, _GL_INLINE, _GL_ATTRIBUTE_ALLOC_SIZE,
   _GL_ATTRIBUTE_MALLOC.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdlib.h>
#if defined __CHERI_PURE_CAPABILITY__
# include <cheri.h>
#endif

_GL_INLINE_HEADER_BEGIN
#ifndef EEALLOC_INLINE
# define EEALLOC_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if MALLOC_0_IS_NONNULL
# define eemalloc malloc
#else
EEALLOC_INLINE void *eemalloc (size_t n)
     _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_ALLOC_SIZE ((1))
     _GL_ATTRIBUTE_DEALLOC_FREE;
EEALLOC_INLINE void *
eemalloc (size_t n)
{
  /* If n is zero, allocate a 1-byte block.  */
  size_t nx = n;
  if (n == 0)
    nx = 1;
  void *ptr = malloc (nx);
# if defined __CHERI_PURE_CAPABILITY__
  if (ptr != NULL)
    ptr = cheri_bounds_set (ptr, n);
# endif
  return ptr;
}
#endif

#if REALLOC_0_IS_NONNULL
# define eerealloc realloc
#else
EEALLOC_INLINE void *eerealloc (void *p, size_t n)
     _GL_ATTRIBUTE_ALLOC_SIZE ((2));
EEALLOC_INLINE void *
eerealloc (void *p, size_t n)
{
  /* Work around realloc glitch by treating a 0 size as if it were 1,
     to avoid undefined behavior in strict C23 platforms,
     and so that returning NULL is equivalent to failing.  */
  void *ptr = realloc (p, n ? n : 1);
# if defined __CHERI_PURE_CAPABILITY__
  if (ptr != NULL)
    ptr = cheri_bounds_set (ptr, n);
# endif
  return ptr;
}
#endif

/* Maybe we should also define variants
    eenmalloc (size_t n, size_t s) - behaves like eemalloc (n * s)
    eezalloc (size_t n) - like eemalloc followed by memset 0
    eecalloc (size_t n, size_t s) - like eemalloc (n * s) followed by memset 0
    eenrealloc (void *p, size_t n, size_t s) - like eerealloc (p, n * s)
   If this would be useful in your application. please speak up.  */


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _EEALLOC_H */
