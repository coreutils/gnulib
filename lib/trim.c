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
#include "mbiterf.h"
#include "xalloc.h"

char *
trim2 (const char *s, int how)
{
  char *d;

  d = strdup (s);

  if (!d)
    xalloc_die ();

  if (MB_CUR_MAX > 1)
    {
      /* Trim leading whitespaces. */
      if (how != TRIM_TRAILING)
        {
          const char *d_end = d + strlen (d);
          mbif_state_t state;
          char *iter;
          for (mbif_init (state), iter = d; mbif_avail (state, iter, d_end); )
            {
              mbchar_t cur = mbif_next (state, iter, d_end);

              if (!mb_isspace (cur))
                break;

              iter += mb_len (cur);
            }

          memmove (d, iter, strlen (iter) + 1);
        }

      /* Trim trailing whitespaces. */
      if (how != TRIM_LEADING)
        {
          char *start_of_spaces = NULL;

          const char *d_end = d + strlen (d);
          mbif_state_t state;
          char *iter;
          for (mbif_init (state), iter = d; mbif_avail (state, iter, d_end); )
            {
              mbchar_t cur = mbif_next (state, iter, d_end);

              if (mb_isspace (cur))
                {
                  if (start_of_spaces == NULL)
                    start_of_spaces = iter;
                }
              else
                start_of_spaces = NULL;

              iter += mb_len (cur);
            }

          if (start_of_spaces != NULL)
            *start_of_spaces = '\0';
        }
    }
  else
    {
      char *p;

      /* Trim leading whitespaces. */
      if (how != TRIM_TRAILING)
        {
          for (p = d; *p && isspace ((unsigned char) *p); p++)
            ;

          memmove (d, p, strlen (p) + 1);
        }

      /* Trim trailing whitespaces. */
      if (how != TRIM_LEADING)
        {
          for (p = d + strlen (d) - 1;
               p >= d && isspace ((unsigned char) *p); p--)
            *p = '\0';
        }
    }

  return d;
}
