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


/* Converts a multibyte character to a 32-bit wide character.  */
#if @GNULIB_MBRTOC32@
# if @REPLACE_MBRTOC32@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef mbrtoc32
#   define mbrtoc32 rpl_mbrtoc32
#  endif
_GL_FUNCDECL_RPL (mbrtoc32, size_t,
                  (char32_t *pc, const char *s, size_t n, mbstate_t *ps));
_GL_CXXALIAS_RPL (mbrtoc32, size_t,
                  (char32_t *pc, const char *s, size_t n, mbstate_t *ps));
# else
#  if !@HAVE_MBRTOC32@
_GL_FUNCDECL_SYS (mbrtoc32, size_t,
                  (char32_t *pc, const char *s, size_t n, mbstate_t *ps));
#  endif
_GL_CXXALIAS_SYS (mbrtoc32, size_t,
                  (char32_t *pc, const char *s, size_t n, mbstate_t *ps));
# endif
_GL_CXXALIASWARN (mbrtoc32);
#elif defined GNULIB_POSIXCHECK
# undef mbrtoc32
# if HAVE_RAW_DECL_MBRTOC32
_GL_WARN_ON_USE (mbrtoc32, "mbrtoc32 is not portable - "
                 "use gnulib module mbrtoc32 for portability");
# endif
#endif


#endif /* _@GUARD_PREFIX@_UCHAR_H */
