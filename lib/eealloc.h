/* Memory allocation with expensive empty allocations.
   Copyright (C) 2003, 2008, 2010-2024 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2003,
   based on prior work by Jim Meyering.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _EEALLOC_H
#define _EEALLOC_H

/* malloc() and realloc() are allowed to return NULL when asked to allocate
   a memory block of 0 bytes; this is not an out-of-memory condition.
   (See ISO C 99 section 7.20.3.)  In some places, this is not welcome,
   because it requires extra checking (so as not to confuse a zero-sized
   allocation with an out-of-memory condition).  This file provides
   malloc()/realloc() workalikes which return non-NULL pointers for
   succeeding zero-sized allocations.  GNU libc already defines malloc()
   and realloc() this way; on such platforms the workalikes are aliased
   to the original malloc()/realloc() functions.  */

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
  /* If n is zero, allocate or keep a 1-byte block.  */
  size_t nx = n;
  if (n == 0)
    nx = 1;
  void *ptr = realloc (p, nx);
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

_GL_INLINE_HEADER_END

#endif /* _EEALLOC_H */
