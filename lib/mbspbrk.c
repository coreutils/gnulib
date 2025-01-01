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
   in the character string ACCEPT.  Return the pointer to it, or NULL if none
   exists.  */
char *
mbspbrk (const char *string, const char *accept)
{
  /* Optimize two cases.  */
  if (accept[0] == '\0')
    return NULL;
  if (accept[1] == '\0')
    return mbschr (string, accept[0]);
  /* General case.  */
  if (MB_CUR_MAX > 1)
    {
      char const *iter = string;
#if GNULIB_MCEL_PREFER
      mcel_t a, g;
      for (; *iter; iter += g.len)
        {
          g = mcel_scanz (iter);
          if (g.len == 1)
            {
              if (mbschr (accept, *iter))
                return (char *) iter;
            }
          else
            for (char const *aiter = accept; *aiter; aiter += a.len)
              {
                a = mcel_scanz (aiter);
                if (mcel_cmp (a, g) == 0)
                  return (char *) iter;
              }
        }
#else
      mbuif_state_t state;
      for (mbuif_init (state); mbuif_avail (state, iter); )
        {
          mbchar_t cur = mbuif_next (state, iter);
          if (mb_len (cur) == 1)
            {
              if (mbschr (accept, *iter))
                return (char *) iter;
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
                    return (char *) iter;
                  aiter += mb_len (acur);
                }
            }
          iter += mb_len (cur);
        }
#endif
      return NULL;
    }
  else
    return strpbrk (string, accept);
}
