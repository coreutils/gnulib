/* Locale dependent memory area transformation for comparison.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

#include <config.h>

/* Specification.  */
#include "amemxfrm.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

char *
amemxfrm (char *s, size_t n, char *resultbuf, size_t *lengthp)
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
        size_t l = strlen (p);

        for (;;)
          {
            size_t k;

            /* A call to strxfrm costs about 20 times more than a call to
               strdup of the result.  Therefore it is worth to try to avoid
               calling strxfrm more than once on a given string, by making
               enough room before calling strxfrm.
               The size of the strxfrm result, k, is likely to be between
               l and 3 * l.  */
            if (3 * l >= allocated - length)
              {
                /* Grow the result buffer.  */
                size_t new_allocated;
                char *new_result;

                new_allocated = length + 3 * l + 1;
                if (new_allocated < 2 * allocated)
                  new_allocated = 2 * allocated;
                if (new_allocated < 64)
                  new_allocated = 64;
                if (result == resultbuf)
                  new_result = (char *) malloc (new_allocated);
                else
                  new_result = (char *) realloc (result, new_allocated);
                if (new_result != NULL)
                  {
                    allocated = new_allocated;
                    result = new_result;
                  }
              }

            errno = 0;
            k = strxfrm (result + length, p, allocated - length);
            if (errno != 0)
              goto fail;
            if (k >= allocated - length)
              {
                /* Grow the result buffer.  */
                size_t new_allocated;
                char *new_result;

                new_allocated = length + k + 1;
                if (new_allocated < 2 * allocated)
                  new_allocated = 2 * allocated;
                if (new_allocated < 64)
                  new_allocated = 64;
                if (result == resultbuf)
                  new_result = (char *) malloc (new_allocated);
                else
                  new_result = (char *) realloc (result, new_allocated);
                if (new_result == NULL)
                  goto out_of_memory_1;
                allocated = new_allocated;
                result = new_result;
              }
            else
              {
                length += k;
                break;
              }
          }

        p = p + l + 1;
        if (p == p_end)
          break;
        result[length] = '\0';
        length++;
      }
  }

  /* Shrink the allocated memory if possible.
     It is not worth calling realloc when length + 1 == allocated; it would
     save just one byte.  */
  if (result != resultbuf && length + 1 < allocated)
    {
      if ((length > 0 ? length : 1) <= *lengthp)
        {
          memcpy (resultbuf, result, length);
          free (result);
          result = resultbuf;
        }
      else
        {
          char *memory = (char *) realloc (result, length > 0 ? length : 1);
          if (memory != NULL)
            result = memory;
        }
    }

  s[n] = orig_sentinel;
  *lengthp = length;
  return result;

 fail:
  if (result != resultbuf)
    free (result);
  s[n] = orig_sentinel;
  return NULL;

 out_of_memory_1:
  if (result != resultbuf)
    free (result);
  s[n] = orig_sentinel;
 out_of_memory_2:
  errno = ENOMEM;
  return NULL;
}
