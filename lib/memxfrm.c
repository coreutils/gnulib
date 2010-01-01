/* Locale dependent memory area transformation for comparison.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "memxfrm.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

char *
memxfrm (char *s, size_t n, char *resultbuf, size_t *lengthp)
{
  /* Result accumulator.  */
  char *result;
  size_t length;
  size_t allocated;

  char orig_sentinel;

  /* Initial memory allocation.  */
  if (resultbuf != NULL && *lengthp > 0)
    {
      result = resultbuf;
      allocated = *lengthp;
    }
  else
    {
      allocated = (n > 0 ? n : 1);
      result = (char *) malloc (allocated);
      if (result == NULL)
        goto out_of_memory_2;
    }
  length = 0;

  /* Add sentinel.byte.  */
  orig_sentinel = s[n];
  s[n] = '\0';

  /* Iterate through S, transforming each NUL terminated segment.
     Accumulate the resulting transformed segments in result, separated by
     NULs.  */
  {
    const char *p_end = s + n + 1;
    const char *p;

    p = s;
    for (;;)
      {
        /* Search next NUL byte.  */
        const char *q = p + strlen (p);

        for (;;)
          {
            size_t k;

            errno = 0;
            k = strxfrm (result + length, p, allocated - length);
            if (errno != 0)
              goto fail;
            if (k >= allocated - length)
              {
                /* Grow the result buffer.  */
                char *new_result;

                allocated = 2 * allocated;
                if (allocated < 64)
                  allocated = 64;
                if (result == resultbuf)
                  new_result = (char *) malloc (allocated);
                else
                  new_result = (char *) realloc (result, allocated);
                if (new_result == NULL)
                  goto out_of_memory_1;
                result = new_result;
              }
            else
              {
                length += k;
                break;
              }
          }

        p = q + 1;
        if (p == p_end)
          break;
        result[length] = '\0';
        length++;
      }
  }

  /* Shrink the allocated memory if possible.  */
  if (result != resultbuf && (length > 0 ? length : 1) < allocated)
    {
      char *memory = (char *) realloc (result, length > 0 ? length : 1);
      if (memory != NULL)
        result = memory;
    }

  s[n] = orig_sentinel;
  *lengthp = length;
  return result;

 fail:
  {
    int saved_errno = errno;
    if (result != resultbuf)
      free (result);
    s[n] = orig_sentinel;
    errno = saved_errno;
    return NULL;
  }

 out_of_memory_1:
  if (result != resultbuf)
    free (result);
  s[n] = orig_sentinel;
 out_of_memory_2:
  errno = ENOMEM;
  return NULL;
}
