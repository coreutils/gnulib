/* Compare two memory areas with possibly different lengths, case-insensitive.
   Copyright (C) 1998-1999, 2005-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009,
   based on earlier glibc code.

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
#include "mbmemcasecmp.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

#if GNULIB_MCEL_PREFER
# include "mcel.h"
#else
# include "mbiterf.h"
#endif

int
mbmemcasecmp (const char *s1, size_t n1, const char *s2, size_t n2)
{
  if (s1 == s2)
    return _GL_CMP (n1, n2);

  const char *iter1 = s1;
  const char *iter2 = s2;
  const char *s1_end = s1 + n1;
  const char *s2_end = s2 + n2;

  if (MB_CUR_MAX > 1)
    {
#if GNULIB_MCEL_PREFER
      while ((iter1 < s1_end) & (iter2 < s2_end))
        {
          mcel_t g1 = mcel_scan (iter1, s1_end); iter1 += g1.len;
          mcel_t g2 = mcel_scan (iter2, s2_end); iter2 += g2.len;
          int cmp = mcel_tocmp (c32tolower, g1, g2);
          if (cmp)
            return cmp;
        }
      return (iter1 < s1_end) - (iter2 < s2_end);
#else
      mbif_state_t state1;
      mbif_init (state1);

      mbif_state_t state2;
      mbif_init (state2);

      while (mbif_avail (state1, iter1, s1_end)
             && mbif_avail (state2, iter2, s2_end))
        {
          mbchar_t cur1 = mbif_next (state1, iter1, s1_end);
          mbchar_t cur2 = mbif_next (state2, iter2, s2_end);
          int cmp = mb_casecmp (cur1, cur2);

          if (cmp != 0)
            return cmp;

          iter1 += mb_len (cur1);
          iter2 += mb_len (cur2);
        }
      if (mbif_avail (state1, iter1, s1_end))
        /* s2 terminated before s1.  */
        return 1;
      if (mbif_avail (state2, iter2, s2_end))
        /* s1 terminated before s2.  */
        return -1;
      return 0;
#endif
    }
  else
    {
      while (iter1 < s1_end && iter2 < s2_end)
        {
          unsigned char c1 = *iter1++;
          unsigned char c2 = *iter2++;
          /* On machines where 'char' and 'int' are types of the same size,
             the difference of two 'unsigned char' values - including
             the sign bit - doesn't fit in an 'int'.  */
          int cmp = UCHAR_MAX <= INT_MAX ? c1 - c2 : _GL_CMP (c1, c2);
          if (cmp != 0)
            {
              c1 = tolower (c1);
              if (c1 != c2)
                {
                  c2 = tolower (c2);
                  cmp = UCHAR_MAX <= INT_MAX ? c1 - c2 : _GL_CMP (c1, c2);
                  if (cmp != 0)
                    return cmp;
                }
            }
        }
      if (iter1 < s1_end)
        /* s2 terminated before s1.  */
        return 1;
      if (iter2 < s2_end)
        /* s1 terminated before s2.  */
        return -1;
      return 0;
    }
}
