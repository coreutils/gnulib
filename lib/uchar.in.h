/* <uchar.h> substitute - 16-bit and 32-bit wide character types.
   Copyright (C) 2019-2022 Free Software Foundation, Inc.

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

/* The __attribute__ feature is available in gcc versions 2.5 and later.
   The attribute __pure__ was added in gcc 2.96.  */
#ifndef _GL_ATTRIBUTE_PURE
# if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96) || defined __clang__
#  define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define _GL_ATTRIBUTE_PURE /* empty */
# endif
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */


#if !(@HAVE_UCHAR_H@ || (defined __cplusplus && @CXX_HAS_UCHAR_TYPES@))

/* A 16-bit variant of wchar_t.
   Note: This type does *NOT* denote UTF-16 units.  (Only on platforms
   on which __STDC_UTF_16__ is defined.)  */
typedef uint_least16_t char16_t;

#elif @GNULIBHEADERS_OVERRIDE_CHAR16_T@

typedef uint_least16_t gl_char16_t;
# define char16_t gl_char16_t

#endif

#if !(@HAVE_UCHAR_H@ || (defined __cplusplus && @CXX_HAS_UCHAR_TYPES@))

/* A 32-bit variant of wchar_t.
   Note: This type does *NOT* denote UTF-32 code points.  (Only on platforms
   on which __STDC_UTF_32__ is defined.)  */
typedef uint_least32_t char32_t;

#elif @GNULIBHEADERS_OVERRIDE_CHAR32_T@

typedef uint_least32_t gl_char32_t;
# define char32_t gl_char32_t

#endif

/* Define if a 'char32_t' can hold more characters than a 'wchar_t'.  */
#if @SMALL_WCHAR_T@                    /* 32-bit AIX, Cygwin, native Windows */
# define _GL_LARGE_CHAR32_T 1
#endif


/* Convert a single-byte character to a 32-bit wide character.  */
#if @GNULIB_BTOC32@
_GL_FUNCDECL_SYS (btoc32, wint_t, (int c) _GL_ATTRIBUTE_PURE);
_GL_CXXALIAS_SYS (btoc32, wint_t, (int c));
_GL_CXXALIASWARN (btoc32);
#endif


/* Test a specific property of a 32-bit wide character.  */
#if @GNULIB_C32ISALNUM@
_GL_FUNCDECL_SYS (c32isalnum, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isalnum, int, (wint_t wc));
_GL_CXXALIASWARN (c32isalnum);
#endif
#if @GNULIB_C32ISALPHA@
_GL_FUNCDECL_SYS (c32isalpha, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isalpha, int, (wint_t wc));
_GL_CXXALIASWARN (c32isalpha);
#endif
#if @GNULIB_C32ISBLANK@
_GL_FUNCDECL_SYS (c32isblank, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isblank, int, (wint_t wc));
_GL_CXXALIASWARN (c32isblank);
#endif
#if @GNULIB_C32ISCNTRL@
_GL_FUNCDECL_SYS (c32iscntrl, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32iscntrl, int, (wint_t wc));
_GL_CXXALIASWARN (c32iscntrl);
#endif
#if @GNULIB_C32ISDIGIT@
_GL_FUNCDECL_SYS (c32isdigit, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isdigit, int, (wint_t wc));
_GL_CXXALIASWARN (c32isdigit);
#endif
#if @GNULIB_C32ISGRAPH@
_GL_FUNCDECL_SYS (c32isgraph, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isgraph, int, (wint_t wc));
_GL_CXXALIASWARN (c32isgraph);
#endif
#if @GNULIB_C32ISLOWER@
_GL_FUNCDECL_SYS (c32islower, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32islower, int, (wint_t wc));
_GL_CXXALIASWARN (c32islower);
#endif
#if @GNULIB_C32ISPRINT@
_GL_FUNCDECL_SYS (c32isprint, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isprint, int, (wint_t wc));
_GL_CXXALIASWARN (c32isprint);
#endif
#if @GNULIB_C32ISPUNCT@
_GL_FUNCDECL_SYS (c32ispunct, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32ispunct, int, (wint_t wc));
_GL_CXXALIASWARN (c32ispunct);
#endif
#if @GNULIB_C32ISSPACE@
_GL_FUNCDECL_SYS (c32isspace, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isspace, int, (wint_t wc));
_GL_CXXALIASWARN (c32isspace);
#endif
#if @GNULIB_C32ISUPPER@
_GL_FUNCDECL_SYS (c32isupper, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isupper, int, (wint_t wc));
_GL_CXXALIASWARN (c32isupper);
#endif
#if @GNULIB_C32ISXDIGIT@
_GL_FUNCDECL_SYS (c32isxdigit, int, (wint_t wc));
_GL_CXXALIAS_SYS (c32isxdigit, int, (wint_t wc));
_GL_CXXALIASWARN (c32isxdigit);
#endif


/* Converts a 32-bit wide character to a multibyte character.  */
#if @GNULIB_C32RTOMB@
# if @REPLACE_C32RTOMB@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef c32rtomb
#   define c32rtomb rpl_c32rtomb
#  endif
_GL_FUNCDECL_RPL (c32rtomb, size_t, (char *s, char32_t wc, mbstate_t *ps));
_GL_CXXALIAS_RPL (c32rtomb, size_t, (char *s, char32_t wc, mbstate_t *ps));
# else
#  if !@HAVE_C32RTOMB@
_GL_FUNCDECL_SYS (c32rtomb, size_t, (char *s, char32_t wc, mbstate_t *ps));
#  endif
_GL_CXXALIAS_SYS (c32rtomb, size_t, (char *s, char32_t wc, mbstate_t *ps));
# endif
_GL_CXXALIASWARN (c32rtomb);
#elif defined GNULIB_POSIXCHECK
# undef c32rtomb
# if HAVE_RAW_DECL_C32RTOMB
_GL_WARN_ON_USE (mbrtoc32, "c32rtomb is not portable - "
                 "use gnulib module c32rtomb for portability");
# endif
#endif


/* Convert a 32-bit wide string to a string.  */
#if @GNULIB_C32SNRTOMBS@
_GL_FUNCDECL_SYS (c32snrtombs, size_t,
                  (char *dest, const char32_t **srcp, size_t srclen, size_t len,
                   mbstate_t *ps)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (c32snrtombs, size_t,
                  (char *dest, const char32_t **srcp, size_t srclen, size_t len,
                   mbstate_t *ps));
_GL_CXXALIASWARN (c32snrtombs);
#endif


/* Convert a 32-bit wide string to a string.  */
#if @GNULIB_C32SRTOMBS@
_GL_FUNCDECL_SYS (c32srtombs, size_t,
                  (char *dest, const char32_t **srcp, size_t len, mbstate_t *ps)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (c32srtombs, size_t,
                  (char *dest, const char32_t **srcp, size_t len,
                   mbstate_t *ps));
_GL_CXXALIASWARN (c32srtombs);
#endif


/* Convert a 32-bit wide string to a string.  */
#if @GNULIB_C32STOMBS@
_GL_FUNCDECL_SYS (c32stombs, size_t,
                  (char *dest, const char32_t *src, size_t len)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (c32stombs, size_t,
                  (char *dest, const char32_t *src, size_t len));
_GL_CXXALIASWARN (c32stombs);
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


/* Convert a string to a 32-bit wide string.  */
#if @GNULIB_MBSNRTOC32S@
_GL_FUNCDECL_SYS (mbsnrtoc32s, size_t,
                  (char32_t *dest, const char **srcp, size_t srclen, size_t len,
                   mbstate_t *ps)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (mbsnrtoc32s, size_t,
                  (char32_t *dest, const char **srcp, size_t srclen, size_t len,
                   mbstate_t *ps));
_GL_CXXALIASWARN (mbsnrtoc32s);
#endif


/* Convert a string to a 32-bit wide string.  */
#if @GNULIB_MBSRTOC32S@
_GL_FUNCDECL_SYS (mbsrtoc32s, size_t,
                  (char32_t *dest, const char **srcp, size_t len, mbstate_t *ps)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (mbsrtoc32s, size_t,
                  (char32_t *dest, const char **srcp, size_t len,
                   mbstate_t *ps));
_GL_CXXALIASWARN (mbsrtoc32s);
#endif


/* Convert a string to a 32-bit wide string.  */
#if @GNULIB_MBSTOC32S@
_GL_FUNCDECL_SYS (mbstoc32s, size_t,
                  (char32_t *dest, const char *src, size_t len)
                  _GL_ARG_NONNULL ((2)));
_GL_CXXALIAS_SYS (mbstoc32s, size_t,
                  (char32_t *dest, const char *src, size_t len));
_GL_CXXALIASWARN (mbstoc32s);
#endif


#endif /* _@GUARD_PREFIX@_UCHAR_H */
