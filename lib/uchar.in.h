/* <uchar.h> substitute - 16-bit and 32-bit wide character types.
   Copyright (C) 2019-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

/*
 * ISO C 11 <uchar.h> for platforms that lack it.
 */

#ifndef _@GUARD_PREFIX@_UCHAR_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#if @HAVE_UCHAR_H@
# @INCLUDE_NEXT@ @NEXT_UCHAR_H@
#endif

/* Get uint_least16_t, uint_least32_t.  */
#include <stdint.h>

/* Get mbstate_t, size_t.  */
#include <wchar.h>

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */


#if !@HAVE_UCHAR_H@

/* A 16-bit variant of wchar_t.
   Note: This type does *NOT* denote UTF-16 units.  (Only on platforms
   on which __STDC_UTF_16__ is defined.)  */
typedef uint_least16_t char16_t;

/* A 32-bit variant of wchar_t.
   Note: This type does *NOT* denote UTF-32 code points.  (Only on platforms
   on which __STDC_UTF_32__ is defined.)  */
typedef uint_least32_t char32_t;

#endif

/* Define if a 'char32_t' can hold more characters than a 'wchar_t'.  */
#if (defined _AIX && !defined __64BIT__) || defined _WIN32 || defined __CYGWIN__
# define _GL_LARGE_CHAR32_T 1
#endif


/* Converts a 32-bit wide character to unibyte character.
   Returns the single-byte representation of WC if it exists,
   or EOF otherwise.  */
#if @GNULIB_C32TOB@
_GL_FUNCDECL_SYS (c32tob, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32tob, int, (wint_t wc));
_GL_CXXALIASWARN (c32tob);
#endif


#endif /* _@GUARD_PREFIX@_UCHAR_H */
