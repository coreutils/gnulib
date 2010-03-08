/* Test of <wchar.h> substitute in C++ mode.
   Copyright (C) 2010 Free Software Foundation, Inc.

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

#include <wchar.h>

#include "signature.h"


#if GNULIB_BTOWC
SIGNATURE_CHECK (GNULIB_NAMESPACE::btowc, wint_t, (int));
#endif

#if GNULIB_WCTOB
SIGNATURE_CHECK (GNULIB_NAMESPACE::wctob, int, (wint_t));
#endif

#if GNULIB_MBSINIT
SIGNATURE_CHECK (GNULIB_NAMESPACE::mbsinit, int, (const mbstate_t *));
#endif

#if GNULIB_MBRTOWC
SIGNATURE_CHECK (GNULIB_NAMESPACE::mbrtowc, size_t,
                 (wchar_t *, const char *, size_t, mbstate_t *));
#endif

#if GNULIB_MBRLEN
SIGNATURE_CHECK (GNULIB_NAMESPACE::mbrlen, size_t,
                 (const char *, size_t, mbstate_t *));
#endif

#if GNULIB_MBSRTOWCS
SIGNATURE_CHECK (GNULIB_NAMESPACE::mbsrtowcs, size_t,
                 (wchar_t *, const char **, size_t, mbstate_t *));
#endif

#if GNULIB_MBSNRTOWCS
SIGNATURE_CHECK (GNULIB_NAMESPACE::mbsnrtowcs, size_t,
                 (wchar_t *, const char **, size_t, size_t, mbstate_t *));
#endif

#if GNULIB_WCRTOMB
SIGNATURE_CHECK (GNULIB_NAMESPACE::wcrtomb, size_t,
                 (char *, wchar_t, mbstate_t *));
#endif

#if GNULIB_WCSRTOMBS
SIGNATURE_CHECK (GNULIB_NAMESPACE::wcsrtombs, size_t,
                 (char *, const wchar_t **, size_t, mbstate_t *));
#endif

#if GNULIB_WCSNRTOMBS
SIGNATURE_CHECK (GNULIB_NAMESPACE::wcsnrtombs, size_t,
                 (char *, const wchar_t **, size_t, size_t, mbstate_t *));
#endif

#if GNULIB_WCWIDTH
SIGNATURE_CHECK (GNULIB_NAMESPACE::wcwidth, int, (wchar_t));
#endif


int
main ()
{
}
