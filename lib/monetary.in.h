/* Wrapper around <monetary.h>.
   Copyright (C) 2017-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _@GUARD_PREFIX@_MONETARY_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_MONETARY_H@
# @INCLUDE_NEXT@ @NEXT_MONETARY_H@
#endif

#ifndef _@GUARD_PREFIX@_MONETARY_H
#define _@GUARD_PREFIX@_MONETARY_H

#if @GNULIB_STRFMON_L@
# if @HAVE_XLOCALE_H@
/* Get locale_t on Mac OS X 10.12.  */
#  include <xlocale.h>
# endif
/* Get locale_t on glibc 2.5.  */
# include <locale.h>
#endif

/* Like in <stdio.h>.  */
#ifndef _GL_ATTRIBUTE_FORMAT
# if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7) || defined __clang__
#  define _GL_ATTRIBUTE_FORMAT(spec) __attribute__ ((__format__ spec))
# else
#  define _GL_ATTRIBUTE_FORMAT(spec) /* empty */
# endif
#endif

/* _GL_ATTRIBUTE_FORMAT_STRFMON
   indicates to GCC that the function takes a format string and arguments,
   where the format string directives are the ones standardized by ISO C99
   and POSIX.  */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
# define _GL_ATTRIBUTE_FORMAT_STRFMON(formatstring_parameter, first_argument) \
   _GL_ATTRIBUTE_FORMAT ((__gnu_strfmon__, formatstring_parameter, first_argument))
#elif __GNUC__ >= 3
# define _GL_ATTRIBUTE_FORMAT_STRFMON(formatstring_parameter, first_argument) \
   _GL_ATTRIBUTE_FORMAT ((__strfmon__, formatstring_parameter, first_argument))
#else
# define _GL_ATTRIBUTE_FORMAT_STRFMON(formatstring_parameter, first_argument) /* empty */
#endif


/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _GL_ARG_NONNULL is copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */

#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_STRFMON_L@
/* Converts a monetary value to a string.
   See the POSIX:2008 specification
   <https://pubs.opengroup.org/onlinepubs/9699919799/functions/strfmon_l.html.  */
# if @REPLACE_STRFMON_L@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define strfmon_l rpl_strfmon_l
#  endif
_GL_FUNCDECL_RPL (strfmon_l, ssize_t,
                  (char *restrict s, size_t maxsize, locale_t locale,
                   const char *restrict format, ...)
                  _GL_ATTRIBUTE_FORMAT_STRFMON (4, 5)
                  _GL_ARG_NONNULL ((4)));
_GL_CXXALIAS_RPL (strfmon_l, ssize_t,
                  (char *restrict s, size_t maxsize, locale_t locale,
                   const char *restrict format, ...));
# else
#  if @HAVE_STRFMON_L@
_GL_CXXALIAS_SYS (strfmon_l, ssize_t,
                  (char *restrict s, size_t maxsize, locale_t locale,
                   const char *restrict format, ...));
#  endif
# endif
# if __GLIBC__ >= 2
_GL_CXXALIASWARN (strfmon_l);
# endif
#elif defined GNULIB_POSIXCHECK
# undef strfmon_l
# if HAVE_RAW_DECL_STRFMON_L
_GL_WARN_ON_USE (strfmon_l, "strfmon_l is buggy on older glibc systems - "
                 "use gnulib module chown for portability");
# endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* _@GUARD_PREFIX@_MONETARY_H */
#endif /* _@GUARD_PREFIX@_MONETARY_H */
