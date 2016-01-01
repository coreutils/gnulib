/* safe-alloc.h: safer memory allocation

   Copyright (C) 2009-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3 of the License, or any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Daniel Berrange <berrange@redhat.com>, 2008 */

#ifndef SAFE_ALLOC_H_
# define SAFE_ALLOC_H_

# include <stdlib.h>

#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define __GNUC_PREREQ(maj, min)                                       \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
# else
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

# ifndef _GL_ATTRIBUTE_RETURN_CHECK
#  if __GNUC_PREREQ (3, 4)
#   define _GL_ATTRIBUTE_RETURN_CHECK __attribute__((__warn_unused_result__))
#  else
#   define _GL_ATTRIBUTE_RETURN_CHECK
#  endif
# endif

/* Don't call these directly - use the macros below */
int
safe_alloc_alloc_n (void *ptrptr, size_t size, size_t count, int zeroed)
  _GL_ATTRIBUTE_RETURN_CHECK;

int
safe_alloc_realloc_n (void *ptrptr, size_t size, size_t count)
  _GL_ATTRIBUTE_RETURN_CHECK;

/**
 * ALLOC:
 * @ptr: pointer to hold address of allocated memory
 *
 * Allocate sizeof(*ptr) bytes of memory and store
 * the address of allocated memory in 'ptr'. Fill the
 * newly allocated memory with zeros.
 *
 * Return -1 on failure to allocate, zero on success
 */
# define ALLOC(ptr)                                     \
  safe_alloc_alloc_n (&(ptr), sizeof (*(ptr)), 1, 1)

/**
 * ALLOC_N:
 * @ptr: pointer to hold address of allocated memory
 * @count: number of elements to allocate
 *
 * Allocate an array of 'count' elements, each sizeof(*ptr)
 * bytes long and store the address of allocated memory in
 * 'ptr'. Fill the newly allocated memory with zeros.
 *
 * Return -1 on failure, 0 on success
 */
# define ALLOC_N(ptr, count)                                    \
  safe_alloc_alloc_n (&(ptr), sizeof (*(ptr)), (count), 1)

/**
 * ALLOC_N_UNINITIALIZED:
 * @ptr: pointer to hold address of allocated memory
 * @count: number of elements to allocate
 *
 * Allocate an array of 'count' elements, each sizeof(*ptr)
 * bytes long and store the address of allocated memory in
 * 'ptr'. Do not initialize the new memory at all.
 *
 * Return -1 on failure to allocate, zero on success
 */
# define ALLOC_N_UNINITIALIZED(ptr, count)                      \
  safe_alloc_alloc_n (&(ptr), sizeof (*(ptr)), (count), 0)

/**
 * REALLOC_N:
 * @ptr: pointer to hold address of allocated memory
 * @count: number of elements to allocate
 *
 * Re-allocate an array of 'count' elements, each sizeof(*ptr)
 * bytes long and store the address of allocated memory in
 * 'ptr'. Fill the newly allocated memory with zeros
 *
 * Return -1 on failure to reallocate, zero on success
 */
# define REALLOC_N(ptr, count)                                  \
  safe_alloc_realloc_n (&(ptr), sizeof (*(ptr)), (count))

/**
 * FREE:
 * @ptr: pointer holding address to be freed
 *
 * Free the memory stored in 'ptr' and update to point
 * to NULL.
 */
# define FREE(ptr)                              \
  do                                            \
    {                                           \
      free (ptr);                               \
      (ptr) = NULL;                             \
    }                                           \
  while (0)

#endif /* SAFE_ALLOC_H_ */
