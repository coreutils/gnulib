/* Next grapheme cluster function.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unigbrk.h"

#include "unistr.h"

const uint32_t *
u32_grapheme_next (const uint32_t *s, const uint32_t *end)
{
  ucs4_t prev;

  if (s == end)
    return NULL;

  u32_mbtouc (&prev, s, end - s);
  for (s++; s != end; s++)
    {
      ucs4_t next;

      u32_mbtouc (&next, s, end - s);
      if (uc_is_grapheme_break (prev, next))
        break;

      prev = next;
    }

  return s;
}
