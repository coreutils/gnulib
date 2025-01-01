/* Locate a substring in a wide string.
   Copyright (C) 1999, 2011-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Eric Blake, 2008.  */

#define AVAILABLE(h, h_l, j, n_l)                       \
  (!MEMCHR0 ((h) + (h_l), (j) + (n_l) - (h_l))          \
   && ((h_l) = (j) + (n_l)))
#include "wcs-two-way.h"

RETURN_TYPE
FUNC (const UNIT *haystack_start, const UNIT *needle_start)
{
  const UNIT *haystack = haystack_start;
  const UNIT *needle = needle_start;
  size_t needle_len; /* Length of NEEDLE.  */
  size_t haystack_len; /* Known minimum length of HAYSTACK.  */
  bool ok = true; /* True if NEEDLE is prefix of HAYSTACK.  */

  /* Determine length of NEEDLE, and in the process, make sure
     HAYSTACK is at least as long (no point processing all of a long
     NEEDLE if HAYSTACK is too short).  */
  while (*haystack && *needle)
    ok &= *haystack++ == *needle++;
  if (*needle)
    return NULL;
  if (ok)
    return (RETURN_TYPE) haystack_start;

  /* Reduce the size of haystack using STRCHR, since it has a smaller
     linear coefficient than the Two-Way algorithm.  */
  needle_len = needle - needle_start;
  haystack = STRCHR (haystack_start + 1, *needle_start);
  if (!haystack || __builtin_expect (needle_len == 1, 0))
    return (RETURN_TYPE) haystack;
  needle -= needle_len;
  haystack_len = (haystack > haystack_start + needle_len ? 1
                  : needle_len + haystack_start - haystack);

  /* Perform the search.  */
  return two_way_short_needle (haystack, haystack_len,
                               needle, needle_len);
}
