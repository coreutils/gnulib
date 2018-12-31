/* safe-alloc.c: safer memory allocation

   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3 of the License, or any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Daniel Berrange <berrange@redhat.com>, 2008 */

#include <config.h>

/* Specification.  */
#include "safe-alloc.h"

#include "xalloc-oversized.h"

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>


/**
 * safe_alloc_alloc_n:
 * @ptrptr: pointer to pointer for address of allocated memory
 * @size: number of bytes to allocate
 * @count: number of elements to allocate
 *
 * Allocate an array of memory 'count' elements long,
 * each with 'size' bytes. Return the address of the
 * allocated memory in 'ptrptr'.  The newly allocated
 * memory is filled with zeros.
 *
 * Return -1 on failure to allocate, zero on success
 */
int
safe_alloc_alloc_n (void *ptrptr, size_t size, size_t count, int zeroed)
{
  if (size == 0 || count == 0)
    {
      *(void **) ptrptr = NULL;
      return 0;
    }

  if (xalloc_oversized (count, size))
    {
      errno = ENOMEM;
      return -1;
    }

  if (zeroed)
    *(void **) ptrptr = calloc (count, size);
  else
    *(void **) ptrptr = malloc (count * size);

  if (*(void **) ptrptr == NULL)
    return -1;
  return 0;
}

/**
 * safe_alloc_realloc_n:
 * @ptrptr: pointer to pointer for address of allocated memory
 * @size: number of bytes to allocate
 * @count: number of elements in array
 *
 * Resize the block of memory in 'ptrptr' to be an array of
 * 'count' elements, each 'size' bytes in length. Update 'ptrptr'
 * with the address of the newly allocated memory. On failure,
 * 'ptrptr' is not changed and still points to the original memory
 * block. The newly allocated memory is filled with zeros.
 *
 * Return -1 on failure to allocate, zero on success
 */
int
safe_alloc_realloc_n (void *ptrptr, size_t size, size_t count)
{
  void *tmp;
  if (size == 0 || count == 0)
    {
      free (*(void **) ptrptr);
      *(void **) ptrptr = NULL;
      return 0;
    }
  if (xalloc_oversized (count, size))
    {
      errno = ENOMEM;
      return -1;
    }
  tmp = realloc (*(void **) ptrptr, size * count);
  if (!tmp)
    return -1;
  *(void **) ptrptr = tmp;
  return 0;
}
