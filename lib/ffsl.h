/* ffsl.h -- find the first set bit in a word.
   Copyright (C) 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Eric Blake.  */

/* This file is meant to be included by ffsl.c and ffsll.c, after
   they have defined FUNC and TYPE.  */

#include <string.h>

#include <limits.h>
#include <strings.h>

#if !defined FUNC || !defined TYPE
# error
#endif

int
FUNC (TYPE i)
{
  int result = 0;
  unsigned TYPE j = i;

  /* GCC has __builtin_ffs, but it is limited to int.  */
  if (!i)
    return 0;
  while (1)
    {
      if ((unsigned int) j)
        return result + ffs ((unsigned int) j);
      j >>= CHAR_BIT * sizeof (unsigned int);
      result += CHAR_BIT * sizeof (unsigned int);
    }
}
