/* Test whether a 32-bit wide character belongs to a specific character class.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <wchar.h>
#include <wctype.h>

#ifdef __CYGWIN__
# include <cygwin/version.h>
#endif

#if GNULIB_defined_mbstate_t
# include "localcharset.h"
# include "streq.h"
#endif

#include "unictype.h"
#include "verify.h"

int
FUNC (wint_t wc)
{
  /* The char32_t encoding of a multibyte character is defined by the way
     mbrtoc32() is defined.  */

#if GNULIB_defined_mbstate_t            /* AIX, IRIX */
  /* mbrtoc32() is defined on top of mbtowc() for the non-UTF-8 locales
     and directly for the UTF-8 locales.  */
  if (wc != WEOF)
    {
      const char *encoding = locale_charset ();
      if (STREQ_OPT (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0, 0))
        return UCS_FUNC (wc);
      else
        return WCHAR_FUNC (wc);
    }
  else
    return 0;

#elif HAVE_WORKING_MBRTOC32             /* glibc */
  /* mbrtoc32() is essentially defined by the system libc.  */

# if defined __GLIBC__
  /* The char32_t encoding of a multibyte character is known to be the same as
     the wchar_t encoding.  */
  return WCHAR_FUNC (wc);
# else
  /* The char32_t encoding of a multibyte character is known to be UCS-4,
     different from the the wchar_t encoding.  */
  if (wc != WEOF)
    return UCS_FUNC (wc);
  else
    return 0;
# endif

#elif _GL_LARGE_CHAR32_T                /* Cygwin, mingw, MSVC */
  /* The wchar_t encoding is UTF-16.
     The char32_t encoding is UCS-4.  */

# if defined __CYGWIN__ && CYGWIN_VERSION_DLL_MAJOR >= 1007
  /* As an extension to POSIX, the iswalnum() function of Cygwin >= 1.7
     supports also wc arguments outside the Unicode BMP, that is, outside
     the 'wchar_t' range.  See
     <https://lists.gnu.org/archive/html/bug-gnulib/2011-02/msg00019.html>
     = <https://cygwin.com/ml/cygwin/2011-02/msg00044.html>.  */
  return WCHAR_FUNC (wc);
# else
  if (wc == WEOF || wc == (wchar_t) wc)
    /* wc is in the range for the isw* functions.  */
    return WCHAR_FUNC (wc);
  else
    return UCS_FUNC (wc);
# endif

#else /* macOS, FreeBSD, NetBSD, OpenBSD, HP-UX, Solaris, Minix, Android */
  /* char32_t and wchar_t are equivalent.  */
  verify (sizeof (char32_t) == sizeof (wchar_t));

  return WCHAR_FUNC (wc);
#endif
}
