/* Test of normalization insensitive comparison of UTF-32 strings.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

#include "uninorm.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZEOF(array) (sizeof (array) / sizeof (array[0]))
#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

int
main ()
{
  /* Empty string.  */
  {
    int cmp;

    ASSERT (u32_normcmp (NULL, 0, NULL, 0, UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);
  }
  {
    static const uint32_t input[] = { 'x', 'y' };
    int cmp;

    ASSERT (u32_normcmp (input, SIZEOF (input), NULL, 0, UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 1);

    ASSERT (u32_normcmp (NULL, 0, input, SIZEOF (input), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);

    ASSERT (u32_normcmp (input, SIZEOF (input), input, SIZEOF (input), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);
  }

  /* Shorter and longer strings.  */
  {
    static const uint32_t input1[] = { 'R', 'e', 'a', 'g', 'a', 'n' };
    static const uint32_t input2[] = { 'R', 'e', 'a', 'g', 'a', 'n', 'o', 'm', 'i', 'c', 's' };
    int cmp;

    ASSERT (u32_normcmp (input1, SIZEOF (input1), input2, SIZEOF (input2), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);

    ASSERT (u32_normcmp (input2, SIZEOF (input2), input1, SIZEOF (input1), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 1);
  }

  /* Normalization effects.  */
  {
    static const uint32_t input1[] = { 'H', 0x00F6, 'h', 'l', 'e' };
    static const uint32_t input2[] = { 'H', 'o', 0x0308, 'h', 'l', 'e' };
    static const uint32_t input3[] = { 'H', 0x00F6, 'h', 'l', 'e', 'n' };
    static const uint32_t input4[] = { 'H', 'o', 0x0308, 'h', 'l', 'e', 'n' };
    static const uint32_t input5[] = { 'H', 'u', 'r', 'z' };
    int cmp;

    ASSERT (u32_normcmp (input1, SIZEOF (input1), input2, SIZEOF (input2), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);

    ASSERT (u32_normcmp (input2, SIZEOF (input2), input1, SIZEOF (input1), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);

    ASSERT (u32_normcmp (input3, SIZEOF (input3), input4, SIZEOF (input4), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);

    ASSERT (u32_normcmp (input4, SIZEOF (input4), input3, SIZEOF (input3), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == 0);

    ASSERT (u32_normcmp (input2, SIZEOF (input2), input3, SIZEOF (input3), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);

    ASSERT (u32_normcmp (input1, SIZEOF (input1), input4, SIZEOF (input4), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);

    ASSERT (u32_normcmp (input1, SIZEOF (input1), input5, SIZEOF (input5), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);

    ASSERT (u32_normcmp (input2, SIZEOF (input2), input5, SIZEOF (input5), UNINORM_NFD, &cmp) == 0);
    ASSERT (cmp == -1);
  }

  return 0;
}
