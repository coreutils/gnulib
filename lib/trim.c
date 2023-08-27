/* Removes leading and/or trailing whitespaces
   Copyright (C) 2006-2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Davide Angelocola <davide.angelocola@gmail.com> */

#include <config.h>

/* Specification.  */
#include "trim.h"

#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "mbchar.h"
#include "mbuiterf.h"
#include "xalloc.h"

char *
trim2 (const char *s, int how)
{
  const char *start = s;
  const char *end;

  if (MB_CUR_MAX > 1)
    {
      mbuif_state_t state;
      mbuif_init (state);

      /* Skip leading whitespace. */
      if (how != TRIM_TRAILING)
        while (mbuif_avail (state, start))
          {
            mbchar_t cur = mbuif_next (state, start);
            if (!mb_isspace (cur))
              break;
            start += mb_len (cur);
          }

      /* Find start of any trailing whitespace.  */
      if (how != TRIM_LEADING)
        for (const char *p = end = start; mbuif_avail (state, p); )
          {
            mbchar_t cur = mbuif_next (state, p);
            p += mb_len (cur);
            if (!mb_isspace (cur))
              end = p;
          }
    }
  else
    {
      /* Skip leading whitespace. */
      if (how != TRIM_TRAILING)
        while (isspace ((unsigned char) *start))
          start++;

      /* Find start of any trailing whitespace.  */
      if (how != TRIM_LEADING)
        for (const char *p = end = start; *p; )
          if (!isspace ((unsigned char) *p++))
            end = p;
    }

  /* Create trimmed copy.  */
  size_t dlen = how == TRIM_LEADING ? strlen (start) : end - start;
  char *d = malloc (dlen + 1);
  if (!d)
    xalloc_die ();
  char *d_end = mempcpy (d, start, dlen);
  *d_end = '\0';

  return d;
}
