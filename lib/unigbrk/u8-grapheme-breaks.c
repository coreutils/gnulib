/* Grapheme cluster breaks function.
   Copyright (C) 2001-2003, 2006-2016 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010,
   based on code written by Bruno Haible <bruno@clisp.org>, 2009.

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
u8_grapheme_breaks (const uint8_t *s, size_t n, char *p)
{
  ucs4_t prev;
  int mblen;

  prev = 0;
  for (; n > 0; s += mblen, p += mblen, n -= mblen)
    {
      ucs4_t next;
      int i;

      mblen = u8_mbtouc (&next, s, n);

      p[0] = uc_is_grapheme_break (prev, next);
      for (i = 1; i < mblen; i++)
        p[i] = 0;

      prev = next;
    }
}
