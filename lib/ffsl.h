/* ffsl.h -- find the first set bit in a word.
   Copyright (C) 2011-2016 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
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
  unsigned TYPE j = i;
  /* Split j into chunks, and look at one chunk after the other.  */
  enum { chunk_bits = CHAR_BIT * sizeof (unsigned int) };
  /* The number of chunks is ceil (sizeof (TYPE) / sizeof (unsigned int))
     = (sizeof (TYPE) - 1) / sizeof (unsigned int) + 1. */
  enum { chunk_count = (sizeof (TYPE) - 1) / sizeof (unsigned int) + 1 };

  if (chunk_count > 1)
    {
      size_t k;

      /* It is tempting to write  if (!j)  here, but if we do this,
         Solaris 10/x86 "cc -O" miscompiles the code.  */
      if (!i)
        return 0;
      /* Unroll the first loop round.  k = 0.  */
      if ((unsigned int) j)
        return ffs ((unsigned int) j);
      /* Generic loop.  */
      for (k = 1; k < chunk_count - 1; k++)
        if ((unsigned int) (j >> (k * chunk_bits)) != 0)
          return k * chunk_bits + ffs ((unsigned int) (j >> (k * chunk_bits)));
    }
  /* Last loop round.  k = chunk_count - 1.  */
  return (chunk_count - 1) * chunk_bits
         + ffs ((unsigned int) (j >> ((chunk_count - 1) * chunk_bits)));
#endif
}
