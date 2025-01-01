/* Test of <fenv.h> substitute.
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

#include "verify.h"

/* Check that the various FE_* macros are defined.  */
int r[] =
  {
#ifdef FE_DOWNWARD
    FE_DOWNWARD,
#endif
#ifdef FE_UPWARD
    FE_UPWARD,
#endif
#ifdef FE_TOWARDZERO
    FE_TOWARDZERO,
#endif
    FE_TONEAREST
  };
int e[] = { FE_DIVBYZERO, FE_INEXACT, FE_INVALID, FE_OVERFLOW, FE_UNDERFLOW };

/* Check that the types are all defined.  */
fenv_t t1;
fexcept_t t2;

/* On many platforms, other FE_* constants are included in FE_ALL_EXCEPT,
   therefore in general
     FE_ALL_EXCEPT == (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)
   does not hold.  */
verify (((FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)
         & ~FE_ALL_EXCEPT)
        == 0);

int
main (void)
{
  /* Ensure no overlap in FE_*. */
  switch (FE_INVALID)
    {
    case FE_DIVBYZERO:
    case FE_INEXACT:
    case FE_INVALID:
    case FE_OVERFLOW:
    case FE_UNDERFLOW:
      ;
    }

  return 0;
}
