/* Grapheme cluster breaks function.
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

void
u32_grapheme_breaks (const uint32_t *s, size_t n, char *p)
{
  ucs4_t prev;
  size_t i;

  prev = 0;
  for (i = 0; i < n; i++)
    {
      ucs4_t next;

      u32_mbtouc (&next, &s[i], 1);

      p[i] = uc_is_grapheme_break (prev, next);

      prev = next;
    }
}
