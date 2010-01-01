/* Search character in UTF-32 string.
   Copyright (C) 1999, 2002, 2006, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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
#include "unistr.h"

uint32_t *
u32_strrchr (const uint32_t *s, ucs4_t uc)
{
  /* Calling u32_strlen and then searching from the other end would cause more
     memory accesses. Avoid that, at the cost of a few more comparisons.  */
  uint32_t *result = NULL;

  for (;; s++)
    {
      if (*s == uc)
        result = (uint32_t *) s;
      if (*s == 0)
        break;
    }
  return result;
}
