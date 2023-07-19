/* Case-insensitive string comparison function.
   Copyright (C) 1998-1999, 2005-2008, 2010-2023 Free Software Foundation, Inc.
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

#include <ctype.h>

#include "mbuiterf.h"

/* Compare the initial segment of the character string STRING consisting of
   at most mbslen (PREFIX) characters with the character string PREFIX,
   ignoring case.  If the two match, return a pointer to the first byte
   after this prefix in STRING.  Otherwise, return NULL.
   Note: This function may, in multibyte locales, return non-NULL if STRING
   is of smaller length than PREFIX!  */
char *
mbspcasecmp (const char *string, const char *prefix)
{
  /* This is essentially the same as
       mbsncasecmp (string, prefix, mbslen (prefix))
     just with small optimizations.  */
  if (string == prefix)
    return (char *) (string + strlen (string));

  /* Be careful not to look at the entire extent of STRING or PREFIX until
     needed.  This is useful because when two strings differ, the difference is
     most often already in the very few first characters.  */
  if (MB_CUR_MAX > 1)
    {
      mbuif_state_t state1;
      const char *iter1;
      mbuif_init (state1);
      iter1 = string;

      mbuif_state_t state2;
      const char *iter2;
      mbuif_init (state2);
      iter2 = prefix;

      while (mbuif_avail (state1, iter1) && mbuif_avail (state2, iter2))
        {
          mbchar_t cur1 = mbuif_next (state1, iter1);
          mbchar_t cur2 = mbuif_next (state2, iter2);
          int cmp = mb_casecmp (cur1, cur2);

          if (cmp != 0)
            return NULL;

          iter1 += mb_len (cur1);
          iter2 += mb_len (cur2);
        }
      if (!mbuif_avail (state2, iter2))
        /* PREFIX equals STRING or is terminated before STRING.  */
        return (char *) iter1;
      else
        /* STRING terminated before PREFIX.  */
        return NULL;
    }
  else
    {
      const unsigned char *p1 = (const unsigned char *) string;
      const unsigned char *p2 = (const unsigned char *) prefix;
      unsigned char c1, c2;

      for (; ; p1++, p2++)
        {
          c1 = tolower (*p1);
          c2 = tolower (*p2);

          if (c2 == '\0' || c1 != c2)
            break;
        }

      if (c2 == '\0')
        /* PREFIX equals STRING or is terminated before STRING.  */
        return (char *) p1;
      else
        /* STRING terminated before PREFIX.  */
        return NULL;
    }
}
