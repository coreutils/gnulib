/* Compute cubic root of double value.
   Copyright (C) 1997, 2012-2022 Free Software Foundation, Inc.

   Contributed by Dirk Alboth <dirka@uni-paderborn.de> and
   Ulrich Drepper <drepper@cygnus.com>, 1997.

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
#include <math.h>

/* MSVC with option -fp:strict refuses to compile constant initializers that
   contain floating-point operations.  Pacify this compiler.  */
#if defined _MSC_VER && !defined __clang__
# pragma fenv_access (off)
#endif

/* Code based on glibc/sysdeps/ieee754/dbl-64/s_cbrt.c.  */

#define CBRT2 1.2599210498948731648             /* 2^(1/3) */
#define SQR_CBRT2 1.5874010519681994748         /* 2^(2/3) */

static const double factor[5] =
{
  1.0 / SQR_CBRT2,
  1.0 / CBRT2,
  1.0,
  CBRT2,
  SQR_CBRT2
};


double
cbrt (double x)
{
  if (isfinite (x) && x != 0.0)
    {
      double xm, ym, u, t2;
      int xe;

      /* Reduce X.  XM now is an range 1.0 to 0.5.  */
      xm = frexp (fabs (x), &xe);

      u = (0.354895765043919860
           + ((1.50819193781584896
               + ((-2.11499494167371287
                   + ((2.44693122563534430
                       + ((-1.83469277483613086
                           + (0.784932344976639262 - 0.145263899385486377 * xm)
                             * xm)
                          * xm))
                      * xm))
                  * xm))
              * xm));

      t2 = u * u * u;

      ym = u * (t2 + 2.0 * xm) / (2.0 * t2 + xm) * factor[2 + xe % 3];

      return ldexp (x > 0.0 ? ym : -ym, xe / 3);
    }
  else
    return x + x;
}
