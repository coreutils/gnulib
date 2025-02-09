/* Case-insensitive string comparison function.
   Copyright (C) 1998-1999, 2005-2008, 2010-2025 Free Software Foundation, Inc.
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

/* Don't use the const-improved function macros in this compilation unit.  */
#define _GL_NO_CONST_GENERICS

#include <config.h>

/* Specification.  */
#include <string.h>

#include <ctype.h>
#include <stdlib.h>

#if GNULIB_MCEL_PREFER
# include "mcel.h"
#else
# include "mbuiterf.h"
#endif

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

  const char *iter1 = string;
  const char *iter2 = prefix;

  /* Be careful not to look at the entire extent of STRING or PREFIX until
     needed.  This is useful because when two strings differ, the difference is
     most often already in the very few first characters.  */
  if (MB_CUR_MAX > 1)
    {
#if GNULIB_MCEL_PREFER
      while (*iter2)
        {
          if (!*iter1)
            return NULL;
          mcel_t g1 = mcel_scanz (iter1); iter1 += g1.len;
          mcel_t g2 = mcel_scanz (iter2); iter2 += g2.len;
          if (mcel_tocmp (c32tolower, g1, g2) != 0)
            return NULL;
        }
      return (char *) iter1;
#else
      mbuif_state_t state1;
      mbuif_init (state1);

      mbuif_state_t state2;
      mbuif_init (state2);

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
#endif
    }
  else
    for (;; iter1++, iter2++)
      {
        unsigned char c2 = *iter2;

        if (c2 == '\0')
          /* PREFIX equals STRING or is terminated before STRING.  */
          return (char *) iter1;

        unsigned char c1 = *iter1;

        if (c1 != c2)
          {
            c1 = tolower (c1);
            if (c1 != c2 && c1 != tolower (c2))
              /* STRING and PREFIX disagree,
                 or STRING terminated before PREFIX.  */
              return NULL;
          }
      }
}
