/* Memory allocation with expensive empty allocations.
   Copyright (C) 2003, 2008, 2010-2022 Free Software Foundation, Inc.
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

#include <stdlib.h>

#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
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
  if (n == 0)
    n = 1;
  return malloc (n);
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
  if (n == 0)
    n = 1;
  return realloc (p, n);
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
