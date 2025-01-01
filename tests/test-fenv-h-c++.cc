/* Test of <fenv.h> substitute in C++ mode.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <fenv.h>

#include "signature.h"


#if GNULIB_TEST_FEGETENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::fegetenv, int, (fenv_t *));
#endif

#if GNULIB_TEST_FESETENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::fesetenv, int, (fenv_t const *));
#endif

#if GNULIB_TEST_FEUPDATEENV
SIGNATURE_CHECK (GNULIB_NAMESPACE::feupdateenv, int, (fenv_t const *));
#endif

#if GNULIB_TEST_FEHOLDEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::feholdexcept, int, (fenv_t *));
#endif

#if GNULIB_TEST_FEGETROUND
SIGNATURE_CHECK (GNULIB_NAMESPACE::fegetround, int, (void));
#endif

#if GNULIB_TEST_FESETROUND
SIGNATURE_CHECK (GNULIB_NAMESPACE::fesetround, int, (int));
#endif

#if GNULIB_TEST_FECLEAREXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::feclearexcept, int, (int));
#endif

#if GNULIB_TEST_FERAISEEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::feraiseexcept, int, (int));
#endif

#if GNULIB_TEST_FETESTEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::fetestexcept, int, (int));
#endif

#if GNULIB_TEST_FESETEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::fesetexcept, int, (int));
#endif

#if GNULIB_TEST_FEENABLEEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::feenableexcept, int, (int));
#endif

#if GNULIB_TEST_FEDISABLEEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::fedisableexcept, int, (int));
#endif

#if GNULIB_TEST_FEGETEXCEPT
SIGNATURE_CHECK (GNULIB_NAMESPACE::fegetexcept, int, (void));
#endif

#if GNULIB_TEST_FEGETEXCEPTFLAG
SIGNATURE_CHECK (GNULIB_NAMESPACE::fegetexceptflag, int, (fexcept_t *, int));
#endif

#if GNULIB_TEST_FESETEXCEPTFLAG
SIGNATURE_CHECK (GNULIB_NAMESPACE::fesetexceptflag, int, (fexcept_t const *, int));
#endif

#if GNULIB_TEST_FETESTEXCEPTFLAG
SIGNATURE_CHECK (GNULIB_NAMESPACE::fetestexceptflag, int, (fexcept_t const *, int));
#endif


int
main ()
{
}
