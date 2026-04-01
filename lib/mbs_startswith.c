/* mbs_startswith function.
   Copyright (C) 2025-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#if GNULIB_MCEL_PREFER
# include "mcel.h"
#else
# include "mbiter.h"
#endif

#include <stdlib.h>

bool
mbs_startswith (const char *string, const char *prefix)
{
  /* First, test whether STRING starts with PREFIX as a byte sequence.
     This is a necessary condition.  */
  if (str_startswith (string, prefix))
    {
      /* It could be that PREFIX ends in an incomplete character and STRING
         continues with a valid character or (if !GNULIB_MCEL_PREFER)
         a longer incomplete character instead.
         In this case, mbs_startswith needs to return false.  */
      size_t mb_max = MB_CUR_MAX;
      if (mb_max == 1)
        /* In unibyte locales, there are no incomplete characters.  */
        return true;
      /* Determine where we can stop the comparison.  */
      size_t p_len = strlen (prefix);
      char const *sp_lim = string + p_len;
      size_t s_len = p_len + strnlen (sp_lim, mb_max);
#if GNULIB_MCEL_PREFER
      char const *s_lim = string + s_len;
      while (string < sp_lim)
        {
          string += mcel_scan (string, s_lim).len;
          if (sp_lim < string)
            return false;
        }
#else
      mbi_iterator_t s_iter;
      mbi_init (s_iter, string, s_len);
      mbi_iterator_t p_iter;
      for (mbi_init (p_iter, prefix, p_len); mbi_avail (p_iter); mbi_advance (p_iter))
        {
          if (!mbi_avail (s_iter))
            return false;
          if (!mb_equal (mbi_cur (s_iter), mbi_cur (p_iter)))
            return false;
          mbi_advance (s_iter);
        }
#endif
      return true;
    }
  return false;
}
