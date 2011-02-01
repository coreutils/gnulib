/* Test of <math.h> substitute in C++ mode.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <math.h>

#include "signature.h"


//SIGNATURE_CHECK (GNULIB_NAMESPACE::acos, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::asin, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::atan, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::atan2, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::cbrt, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::copysign, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::cos, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::cosh, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::erf, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::erfc, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::exp, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::fabs, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::fmod, double, (double, double));
#if GNULIB_TEST_FREXP
SIGNATURE_CHECK (GNULIB_NAMESPACE::frexp, double, (double, int *));
#endif
//SIGNATURE_CHECK (GNULIB_NAMESPACE::hypot, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::j0, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::j1, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::jn, double, (int, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::ldexp, double, (double, int));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::lgamma, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::log10, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::log1p, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::logb, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::log, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::modf, double, (double, double *));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::nextafter, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::pow, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::remainder, double, (double, double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::rint, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::sin, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::sinh, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::sqrt, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::tan, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::tanh, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::y0, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::y1, double, (double));
//SIGNATURE_CHECK (GNULIB_NAMESPACE::yn, double, (int, double));

#if GNULIB_TEST_ACOSL
SIGNATURE_CHECK (GNULIB_NAMESPACE::acosl, long double, (long double));
#endif

#if GNULIB_TEST_ASINL
SIGNATURE_CHECK (GNULIB_NAMESPACE::asinl, long double, (long double));
#endif

#if GNULIB_TEST_ATANL
SIGNATURE_CHECK (GNULIB_NAMESPACE::atanl, long double, (long double));
#endif

#if GNULIB_TEST_CEILF
SIGNATURE_CHECK (GNULIB_NAMESPACE::ceilf, float, (float));
#endif

#if GNULIB_TEST_CEIL
SIGNATURE_CHECK (GNULIB_NAMESPACE::ceil, double, (double));
#endif

#if GNULIB_TEST_CEILL
SIGNATURE_CHECK (GNULIB_NAMESPACE::ceill, long double, (long double));
#endif

#if GNULIB_TEST_COSL
SIGNATURE_CHECK (GNULIB_NAMESPACE::cosl, long double, (long double));
#endif

#if GNULIB_TEST_EXPL
SIGNATURE_CHECK (GNULIB_NAMESPACE::expl, long double, (long double));
#endif

#if GNULIB_TEST_FLOORF
SIGNATURE_CHECK (GNULIB_NAMESPACE::floorf, float, (float));
#endif

#if GNULIB_TEST_FLOOR
SIGNATURE_CHECK (GNULIB_NAMESPACE::floor, double, (double));
#endif

#if GNULIB_TEST_FLOORL
SIGNATURE_CHECK (GNULIB_NAMESPACE::floorl, long double, (long double));
#endif

#if GNULIB_TEST_FREXPL
SIGNATURE_CHECK (GNULIB_NAMESPACE::frexpl, long double, (long double, int *));
#endif

#if GNULIB_TEST_LDEXPL
SIGNATURE_CHECK (GNULIB_NAMESPACE::ldexpl, long double, (long double, int));
#endif

#if GNULIB_TEST_LOGL
SIGNATURE_CHECK (GNULIB_NAMESPACE::logl, long double, (long double));
#endif

#if GNULIB_TEST_ROUNDF
SIGNATURE_CHECK (GNULIB_NAMESPACE::roundf, float, (float));
#endif

#if GNULIB_TEST_ROUND
SIGNATURE_CHECK (GNULIB_NAMESPACE::round, double, (double));
#endif

#if GNULIB_TEST_ROUNDL
SIGNATURE_CHECK (GNULIB_NAMESPACE::roundl, long double, (long double));
#endif

#if GNULIB_TEST_SINL
SIGNATURE_CHECK (GNULIB_NAMESPACE::sinl, long double, (long double));
#endif

#if GNULIB_TEST_SQRTL
SIGNATURE_CHECK (GNULIB_NAMESPACE::sqrtl, long double, (long double));
#endif

#if GNULIB_TEST_TANL
SIGNATURE_CHECK (GNULIB_NAMESPACE::tanl, long double, (long double));
#endif

#if GNULIB_TEST_TRUNCF
SIGNATURE_CHECK (GNULIB_NAMESPACE::truncf, float, (float));
#endif

#if GNULIB_TEST_TRUNC
SIGNATURE_CHECK (GNULIB_NAMESPACE::trunc, double, (double));
#endif

#if GNULIB_TEST_TRUNCL
SIGNATURE_CHECK (GNULIB_NAMESPACE::truncl, long double, (long double));
#endif


int
main ()
{
}
