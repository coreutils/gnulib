/* imaxdiv() function: division of 'intmax_t'.
   Copyright (C) 2006, 2009-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <inttypes.h>

#include <stdlib.h>

imaxdiv_t
imaxdiv (intmax_t numer, intmax_t denom)
{
  imaxdiv_t result;

  result.quot = numer / denom;
  result.rem = numer % denom;

  /* Verify the requirements of ISO C 99 section 6.5.5 paragraph 6:
     "When integers are divided, the result of the / operator is the
      algebraic quotient with any fractional part discarded.  (This is
      often called "truncation toward zero".)  If the quotient a/b is
      representable, the expression (a/b)*b + a%b shall equal a."  */
  if (!(denom == 0
        || (INTMAX_MIN + INTMAX_MAX < 0
            && denom == -1
            && numer < - INTMAX_MAX)))
    {
      if (!(result.quot * denom + result.rem == numer))
        /* The compiler's implementation of / and % is broken.  */
        abort ();
      if (!(numer >= 0
            ? result.rem >= 0
              && (denom >= 0
                  ? result.rem < denom
                  : /* Don't write  result.rem < - denom,
                       as it gives integer overflow if denom == INTMAX_MIN.  */
                    - result.rem > denom)
            : result.rem <= 0
              && (denom >= 0
                  ? result.rem > - denom
                  : result.rem > denom)))
        /* The compiler's implementation of / and % may be ok according to
           C89, but not to C99.  Please report this to <bug-gnulib@ngu.org>.
           This might be a big portability problem.  */
        abort ();
    }

  return result;
}
