/* Case-insensitive string comparison function.
   Copyright (C) 1998-1999, 2005-2008, 2010-2022 Free Software Foundation, Inc.
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

#include "mbuiter.h"

#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

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
      mbui_iterator_t iter1;
      mbui_iterator_t iter2;

      mbui_init (iter1, string);
      mbui_init (iter2, prefix);

      while (mbui_avail (iter1) && mbui_avail (iter2))
        {
          int cmp = mb_casecmp (mbui_cur (iter1), mbui_cur (iter2));

          if (cmp != 0)
            return NULL;

          mbui_advance (iter1);
          mbui_advance (iter2);
        }
      if (!mbui_avail (iter2))
        /* PREFIX equals STRING or is terminated before STRING.  */
        return (char *) mbui_cur_ptr (iter1);
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
          c1 = TOLOWER (*p1);
          c2 = TOLOWER (*p2);

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
