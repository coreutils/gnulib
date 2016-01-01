/* Previous grapheme cluster function.
   Copyright (C) 2010-2016 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unigbrk.h"

#include "unistr.h"

const uint16_t *
u16_grapheme_prev (const uint16_t *s, const uint16_t *start)
{
  ucs4_t next;

  if (s == start)
    return NULL;

  s = u16_prev (&next, s, start);
  while (s != start)
    {
      const uint16_t *prev_s;
      ucs4_t prev;

      prev_s = u16_prev (&prev, s, start);
      if (prev_s == NULL)
        {
          /* Ill-formed UTF-16 encoding. */
          return start;
        }

      if (uc_is_grapheme_break (prev, next))
        break;

      s = prev_s;
      next = prev;
    }

  return s;
}
