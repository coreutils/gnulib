/* safe-alloc.h: safer memory allocation

   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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

/* Written by Daniel Berrange and Paul Eggert.  */

#ifndef SAFE_ALLOC_H_
#define SAFE_ALLOC_H_

#include <stdlib.h>

#ifndef _GL_INLINE_HEADER_BEGIN
   #error "Please include config.h first."
#endif
_GL_INLINE_HEADER_BEGIN
#ifndef SAFE_ALLOC_INLINE
# define SAFE_ALLOC_INLINE _GL_INLINE
#endif

/* Don't call these directly - use the macros below.  */
SAFE_ALLOC_INLINE void *
safe_alloc_realloc_n (void *ptr, size_t count, size_t size)
{
  if (count == 0 || size == 0)
    count = size = 1;
  return reallocarray (ptr, count, size);
}
_GL_ATTRIBUTE_NODISCARD SAFE_ALLOC_INLINE int
safe_alloc_check (void *ptr)
{
  /* Return 0 if the allocation was successful, -1 otherwise.  */
  return -!ptr;
}

/**
 * ALLOC:
 * @ptr: pointer to allocated memory
 *
 * Allocate sizeof *ptr bytes of memory and store
 * the address of allocated memory in 'ptr'.  Fill the
 * newly allocated memory with zeros.
 *
 * Return -1 on failure to allocate, zero on success.
 */
#define ALLOC(ptr) ALLOC_N (ptr, 1)

/**
 * ALLOC_N:
 * @ptr: pointer to allocated memory
 * @count: number of elements to allocate
 *
 * Allocate an array of 'count' elements, each sizeof *ptr
 * bytes long and store the address of allocated memory in
 * 'ptr'.  Fill the newly allocated memory with zeros.
 *
 * Return -1 on failure, 0 on success.
 */
#define ALLOC_N(ptr, count) \
  safe_alloc_check ((ptr) = calloc (count, sizeof *(ptr)))

/**
 * ALLOC_N_UNINITIALIZED:
 * @ptr: pointer to allocated memory
 * @count: number of elements to allocate
 *
 * Allocate an array of 'count' elements, each sizeof *ptr
 * bytes long and store the address of allocated memory in
 * 'ptr'.  Do not initialize the new memory at all.
 *
 * Return -1 on failure to allocate, zero on success.
 */
#define ALLOC_N_UNINITIALIZED(ptr, count) \
  safe_alloc_check ((ptr) = safe_alloc_realloc_n (NULL, count, sizeof *(ptr)))

/**
 * REALLOC_N:
 * @ptr: pointer to allocated memory
 * @count: number of elements to allocate
 *
 * Re-allocate an array of 'count' elements, each sizeof *ptr
 * bytes long and store the address of allocated memory in
 * 'ptr'.  Fill the newly allocated memory with zeros.
 *
 * Return -1 on failure to reallocate, zero on success.
 */
#define REALLOC_N(ptr, count) \
  safe_alloc_check ((ptr) = safe_alloc_realloc_n (ptr, count, sizeof *(ptr)))

/**
 * FREE:
 * @ptr: pointer holding address to be freed
 *
 * Free the memory stored in 'ptr' and update to point
 * to NULL.
 */
#define FREE(ptr) ((void) (free (ptr), (ptr) = NULL))

_GL_INLINE_HEADER_END

#endif /* SAFE_ALLOC_H_ */
