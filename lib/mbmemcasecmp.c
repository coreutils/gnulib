/* Compare two memory areas with possibly different lengths, case-insensitive.
   Copyright (C) 1998-1999, 2005-2022 Free Software Foundation, Inc.
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

#include "mbiter.h"

#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

int
mbmemcasecmp (const char *s1, size_t n1, const char *s2, size_t n2)
{
  if (s1 == s2)
    return _GL_CMP (n1, n2);

  if (MB_CUR_MAX > 1)
    {
      mbi_iterator_t iter1;
      mbi_iterator_t iter2;

      mbi_init (iter1, s1, n1);
      mbi_init (iter2, s2, n2);

      while (mbi_avail (iter1) && mbi_avail (iter2))
        {
          int cmp = mb_casecmp (mbi_cur (iter1), mbi_cur (iter2));

          if (cmp != 0)
            return cmp;

          mbi_advance (iter1);
          mbi_advance (iter2);
        }
      if (mbi_avail (iter1))
        /* s2 terminated before s1.  */
        return 1;
      if (mbi_avail (iter2))
        /* s1 terminated before s2.  */
        return -1;
      return 0;
    }
  else
    {
      const unsigned char *s1_end = (const unsigned char *) (s1 + n1);
      const unsigned char *s2_end = (const unsigned char *) (s2 + n2);
      const unsigned char *p1 = (const unsigned char *) s1;
      const unsigned char *p2 = (const unsigned char *) s2;

      while (p1 < s1_end && p2 < s2_end)
        {
          unsigned char c1 = TOLOWER (*p1);
          unsigned char c2 = TOLOWER (*p2);
          if (c1 != c2)
            {
              if (UCHAR_MAX <= INT_MAX)
                return c1 - c2;
              else
                /* On machines where 'char' and 'int' are types of the same
                   size, the difference of two 'unsigned char' values
                   - including the sign bit - doesn't fit in an 'int'.  */
                return _GL_CMP (c1, c2);
            }
          ++p1;
          ++p2;
        }
      if (p1 < s1_end)
        /* s2 terminated before s1.  */
        return 1;
      if (p2 < s2_end)
        /* s1 terminated before s2.  */
        return -1;
      return 0;
    }
}
