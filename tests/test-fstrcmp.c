/* Test of fuzzy string comparison.
   Copyright (C) 2007 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include "fstrcmp.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          abort ();							     \
        }								     \
    }									     \
  while (0)

static bool
check_fstrcmp (const char *string1, const char *string2, double expected)
{
  /* The use of 'volatile' guarantees that excess precision bits are dropped
     before the addition and before the following comparison at the caller's
     site.  It is necessary on x86 systems where double-floats are not IEEE
     compliant by default, to avoid that msgmerge results become platform and
     compiler option dependent.  'volatile' is a portable alternative to gcc's
     -ffloat-store option.  */
  volatile double result = fstrcmp (string1, string2);
  return (result == expected);
}

int
main ()
{
  ASSERT (check_fstrcmp ("Langstrumpf", "Langstrumpf", 1.0));
  ASSERT (check_fstrcmp ("Levenshtein", "Levenstein", 20./21.));
  ASSERT (check_fstrcmp ("Levenstein", "Levenshtein", 20./21.));
  ASSERT (check_fstrcmp ("xy", "yx", 1./2.));
  ASSERT (check_fstrcmp ("George Bush", "Abraham Lincoln", 2./13.));
  ASSERT (check_fstrcmp ("George Bush", "George \"Bugs\" Moran", 2./3.));

  return 0;
}
