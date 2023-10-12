/* Tests of signalling not-a-number.
   Copyright (C) 2023 Free Software Foundation, Inc.

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
#include "snan.h"

#if HAVE_FE_INVALID

# include <fenv.h>

# include "macros.h"

float volatile resultf;
double volatile resultd;
long double volatile resultl;

int
main ()
{
  /* Fetch the NaN values before we start watching out for FE_INVALID
     exceptions, because the division 0.0 / 0.0 itself also raises an
     FE_INVALID exception.
     The use of 'volatile' prevents the compiler from doing constant-folding
     optimizations on these values.  An alternative, for GCC only, would be
     the command-line option '-fsignaling-nans'.  */
  float volatile nanf = SNaNf ();
  double volatile nand = SNaNd ();
  long double volatile nanl = SNaNl ();

  /* Check that the values are really signalling.  */
  {
    feclearexcept (FE_INVALID);
    resultf = nanf + 42.0f;
    ASSERT (fetestexcept (FE_INVALID));
  }
  {
    feclearexcept (FE_INVALID);
    resultd = nand + 42.0;
    ASSERT (fetestexcept (FE_INVALID));
  }
  {
    feclearexcept (FE_INVALID);
    resultl = nanl + 42.0L;
    ASSERT (fetestexcept (FE_INVALID));
  }

  return 0;
}

#else

/* No <fenv.h> available.
   We could use the various alternative approaches from
   libgfortran/config/fpu-*.h, but that's not worth it.  */

#include <stdio.h>

int
main ()
{
  fputs ("Skipping test: feclearexcept, fetestexcept, FE_INVALID not available\n", stderr);
  return 77;
}

#endif
