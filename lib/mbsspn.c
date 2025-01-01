/* Searching a string for a character outside a given set of characters.
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
   not in the character string REJECT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.  */
size_t
mbsspn (const char *string, const char *reject)
{
  /* Optimize two cases.  */
  if (reject[0] == '\0')
    return 0;
  if (reject[1] == '\0')
    {
      unsigned char uc = (unsigned char) reject[0];
      const char *iter = string;

      if (MB_CUR_MAX > 1)
        {
#if GNULIB_MCEL_PREFER
          for (mcel_t g; *iter; iter += g.len)
            {
              g = mcel_scanz (iter);
              if (! (g.len == 1 && (unsigned char) *iter == uc))
                break;
            }
#else
          mbuif_state_t state;
          for (mbuif_init (state); mbuif_avail (state, iter); )
            {
              mbchar_t cur = mbuif_next (state, iter);
              if (!(mb_len (cur) == 1 && (unsigned char) *iter == uc))
                break;
              iter += mb_len (cur);
            }
#endif
        }
      else
        {
          for (; *iter != '\0'; iter++)
            if ((unsigned char) *iter != uc)
              break;
        }
      return iter - string;
    }
  /* General case.  */
  if (MB_CUR_MAX > 1)
    {
#if GNULIB_MCEL_PREFER
      for (size_t i = 0; ; )
        {
          char c = string[i];
          if (!c)
            return i;
          mcel_t g = mcel_scanz (string + i);
          if (g.len == 1)
            {
              if (!mbschr (reject, c))
                return i;
            }
          else
            {
              for (char const *aiter = reject; ; )
                {
                  if (!*aiter)
                    return i;
                  mcel_t a = mcel_scanz (aiter);
                  if (mcel_cmp (a, g) == 0)
                    break;
                  aiter += a.len;
                }
            }
          i += g.len;
        }
#else
      mbuif_state_t state;
      const char *iter;
      for (mbuif_init (state), iter = string; mbuif_avail (state, iter); )
        {
          mbchar_t cur = mbuif_next (state, iter);
          if (mb_len (cur) == 1)
            {
              if (mbschr (reject, *iter) == NULL)
                goto found;
            }
          else
            {
              mbuif_state_t astate;
              const char *aiter;
              for (mbuif_init (astate), aiter = reject; ; )
                {
                  if (!mbuif_avail (astate, aiter))
                    goto found;
                  mbchar_t acur = mbuif_next (astate, aiter);
                  if (mb_equal (acur, cur))
                    break;
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
    return strspn (string, reject);
}
