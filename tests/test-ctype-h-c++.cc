/* Test of <ctype.h> substitute in C++ mode.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <ctype.h>

#include "signature.h"


#if GNULIB_TEST_ISALNUM_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isalnum_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISALPHA_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isalpha_l, int, (int, locale_t));
#endif

#if 0
SIGNATURE_CHECK (GNULIB_NAMESPACE::isblank, int, (int));
#endif

#if GNULIB_TEST_ISBLANK_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isblank_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISCNTRL_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::iscntrl_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISDIGIT_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isdigit_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISGRAPH_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isgraph_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISLOWER_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::islower_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISPRINT_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isprint_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISPUNCT_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::ispunct_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISSPACE_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isspace_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISUPPER_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isupper_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_ISXDIGIT_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::isxdigit_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_TOLOWER_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::tolower_l, int, (int, locale_t));
#endif

#if GNULIB_TEST_TOUPPER_L
SIGNATURE_CHECK (GNULIB_NAMESPACE::toupper_l, int, (int, locale_t));
#endif


int
main ()
{
}
