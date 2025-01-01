/* Test of turning floating-point exceptions into traps (signals).
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include <stdio.h>

#include "macros.h"

/* Check of the Boole-an algebra.  */

/* Converts 5 bits to an exceptions bit mask.  */
static int
uint_to_exceptions (unsigned int a)
{
  return (a & 0x01 ? FE_INVALID : 0)
         | (a & 0x02 ? FE_DIVBYZERO : 0)
         | (a & 0x04 ? FE_OVERFLOW : 0)
         | (a & 0x08 ? FE_UNDERFLOW : 0)
         | (a & 0x10 ? FE_INEXACT : 0);
}

int
main ()
{
  unsigned int a, b;

  /* Run through all possible valid arguments to feenableexcept and
     fedisableexcept.
     An alternative way of coding this iteration, without the uint_to_exceptions
     function, would be using the trick from
     Jörg Arndt: Matters Computational <https://www.jjj.de/fxt/fxtbook.pdf>
     § 1.25.1 Generating bit subsets of a given word  */
  for (a = 0; a < 0x20; a++)
    for (b = 0; b < 0x20; b++)
      {
        unsigned int c = a & ~b;

        if (fedisableexcept (FE_ALL_EXCEPT) == -1
            || feenableexcept (uint_to_exceptions (a)) == -1)
          {
            if (test_exit_status != EXIT_SUCCESS)
              return test_exit_status;
            fputs ("Skipping test: trapping floating-point exceptions are not supported on this machine.\n", stderr);
            return 77;
          }
        ASSERT (fedisableexcept (uint_to_exceptions (b))
                == uint_to_exceptions (a));
        /* Check fegetexcept.  */
        ASSERT (fegetexcept () == uint_to_exceptions (c));
        /* Check the return value of feenableexcept.  It should be consistent
           with fegetexcept.  */
        ASSERT (feenableexcept (0) == uint_to_exceptions (c));
      }

  return test_exit_status;
}
