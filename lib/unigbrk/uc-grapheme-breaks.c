/* Grapheme cluster breaks function.
   Copyright (C) 2010-2020 Free Software Foundation, Inc.
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unigbrk.h"

#include <string.h>

/* This is similar to u32_mbtouc_unsafe(), but doesn't check invalid
   characters.  */
static int
uc_grapheme_breaks_mbtouc (ucs4_t *puc, const ucs4_t *s, size_t n)
{
  *puc = *s;
  return 1;
}

#define FUNC uc_grapheme_breaks
#define UNIT ucs4_t
#define U_MBTOUC uc_grapheme_breaks_mbtouc
#include "u-grapheme-breaks.h"
