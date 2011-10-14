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
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) && defined GCC_BUILTIN
  return GCC_BUILTIN (i);
#else
  if (sizeof (TYPE) == sizeof (int))
    return ffs (i);
  else
    {
      unsigned TYPE j = i;
      /* Split j into chunks, and look at one chunk after the other.  */
      /* Define chunk_bits so as to avoid a GCC warning
           "right shift count >= width of type"
         if sizeof (TYPE) == sizeof (int).  */
      enum
        {
          chunk_bits = (sizeof (TYPE) != sizeof (int)
                        ? CHAR_BIT * sizeof (unsigned int)
                        : 0)
        };
      int result = 0;

      /* It is tempting to write  if (!j)  here, but if we do this,
         Solaris 10/x86 "cc -O" miscompiles the code.  */
      if (!i)
        return 0;
      while (1)
        {
          if ((unsigned int) j)
            return result + ffs ((unsigned int) j);
          j >>= chunk_bits;
          result += chunk_bits;
        }
    }
#endif
}
