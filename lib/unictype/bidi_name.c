/* Bidi categories of Unicode characters.
   Copyright (C) 2002, 2006, 2009-2010 Free Software Foundation, Inc.
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
#include "unictype.h"

static const char u_bidi_category_name[19][4] =
{
  "L",  "LRE", "LRO", "R",   "AL", "RLE", "RLO", "PDF", "EN", "ES",
  "ET", "AN",  "CS",  "NSM", "BN", "B",   "S",   "WS",  "ON"
};

const char *
uc_bidi_category_name (int category)
{
  if (category >= 0 && category < sizeof (u_bidi_category_name) / sizeof (u_bidi_category_name[0]))
    return u_bidi_category_name[category];
  return NULL;
}
