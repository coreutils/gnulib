/* Searching a string for a character among a given set of characters.
   Copyright (C) 1999, 2002, 2006-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include <stdlib.h>

#if GNULIB_MCEL_PREFER
# include "mcel.h"
#else
# include "mbuiterf.h"
#endif

/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.  */
size_t
mbscspn (const char *string, const char *accept)
{
  /* Optimize two cases.  */
  if (accept[0] == '\0')
    return strlen (string);
  if (accept[1] == '\0')
    {
      const char *ptr = mbschr (string, accept[0]);
      return (ptr != NULL ? ptr - string : strlen (string));
    }
  /* General case.  */
  if (MB_CUR_MAX > 1)
    {
#if GNULIB_MCEL_PREFER
      mcel_t a, g;
      size_t i;
      for (i = 0; string[i]; i += g.len)
        {
          g = mcel_scanz (string + i);
          if (g.len == 1)
            {
              if (mbschr (accept, string[i]))
                return i;
            }
          else
            for (char const *aiter = accept; *aiter; aiter += a.len)
              {
                a = mcel_scanz (aiter);
                if (mcel_cmp (g, a) == 0)
                  return i;
              }
        }
      return i;
#else
      mbuif_state_t state;
      const char *iter;
      for (mbuif_init (state), iter = string; mbuif_avail (state, iter); )
        {
          mbchar_t cur = mbuif_next (state, iter);
          if (mb_len (cur) == 1)
            {
              if (mbschr (accept, *iter))
                goto found;
            }
          else
            {
              mbuif_state_t astate;
              const char *aiter;
              for (mbuif_init (astate), aiter = accept;
                   mbuif_avail (astate, aiter); )
                {
                  mbchar_t acur = mbuif_next (astate, aiter);
                  if (mb_equal (acur, cur))
                    goto found;
                  aiter += mb_len (acur);
                }
            }
          iter += mb_len (cur);
        }
     found:
      return iter - string;
#endif
    }
  else
    return strcspn (string, accept);
}
