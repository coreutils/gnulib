/* A substitute for ISO C99 <wchar.h>, for platforms that have issues.

   Copyright (C) 2007-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Eric Blake.  */

/*
 * ISO C 99 <wchar.h> for platforms that have issues.
 * <http://www.opengroup.org/susv3xbd/wchar.h.html>
 *
 * For now, this just ensures proper prerequisite inclusion order and
 * the declaration of wcwidth().
 */

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif

#if defined __need_mbstate_t || defined __need_wint_t || (defined __hpux && ((defined _INTTYPES_INCLUDED && !defined strtoimax) || defined _GL_JUST_INCLUDE_SYSTEM_WCHAR_H)) || defined _GL_ALREADY_INCLUDING_WCHAR_H
/* Special invocation convention:
   - Inside glibc and uClibc header files.
   - On HP-UX 11.00 we have a sequence of nested includes
     <wchar.h> -> <stdlib.h> -> <stdint.h>, and the latter includes <wchar.h>,
     once indirectly <stdint.h> -> <sys/types.h> -> <inttypes.h> -> <wchar.h>
     and once directly.  In both situations 'wint_t' is not yet defined,
     therefore we cannot provide the function overrides; instead include only
     the system's <wchar.h>.
   - On IRIX 6.5, similarly, we have an include <wchar.h> -> <wctype.h>, and
     the latter includes <wchar.h>.  But here, we have no way to detect whether
     <wctype.h> is completely included or is still being included.  */

#@INCLUDE_NEXT@ @NEXT_WCHAR_H@

#else
/* Normal invocation convention.  */

#ifndef _GL_WCHAR_H

#define _GL_ALREADY_INCLUDING_WCHAR_H

/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be
   included before <wchar.h>.
   But avoid namespace pollution on glibc systems.  */
#ifndef __GLIBC__
# include <stddef.h>
# include <stdio.h>
# include <time.h>
#endif

/* Include the original <wchar.h> if it exists.
   Some builds of uClibc lack it.  */
/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_WCHAR_H@
# @INCLUDE_NEXT@ @NEXT_WCHAR_H@
#endif

#undef _GL_ALREADY_INCLUDING_WCHAR_H

#ifndef _GL_WCHAR_H
#define _GL_WCHAR_H

/* The definition of _GL_ARG_NONNULL is copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */

#ifdef __cplusplus
extern "C" {
#endif


/* Define wint_t.  (Also done in wctype.in.h.)  */
#if !@HAVE_WINT_T@ && !defined wint_t
# define wint_t int
# ifndef WEOF
#  define WEOF -1
# endif
#endif


/* Override mbstate_t if it is too small.
   On IRIX 6.5, sizeof (mbstate_t) == 1, which is not sufficient for
   implementing mbrtowc for encodings like UTF-8.  */
#if !(@HAVE_MBSINIT@ && @HAVE_MBRTOWC@) || @REPLACE_MBSTATE_T@
typedef int rpl_mbstate_t;
# undef mbstate_t
# define mbstate_t rpl_mbstate_t
# define GNULIB_defined_mbstate_t 1
#endif


/* Convert a single-byte character to a wide character.  */
#if @GNULIB_BTOWC@
# if @REPLACE_BTOWC@
#  undef btowc
#  define btowc rpl_btowc
# endif
# if !@HAVE_BTOWC@ || @REPLACE_BTOWC@
extern wint_t btowc (int c);
# endif
#elif defined GNULIB_POSIXCHECK
# undef btowc
# if HAVE_RAW_DECL_BTOWC
_GL_WARN_ON_USE (btowc, "btowc is unportable - "
                 "use gnulib module btowc for portability");
# endif
#endif


/* Convert a wide character to a single-byte character.  */
#if @GNULIB_WCTOB@
# if @REPLACE_WCTOB@
#  undef wctob
#  define wctob rpl_wctob
# endif
# if (!defined wctob && !@HAVE_DECL_WCTOB@) || @REPLACE_WCTOB@
/* wctob is provided by gnulib, or wctob exists but is not declared.  */
extern int wctob (wint_t wc);
# endif
#elif defined GNULIB_POSIXCHECK
# undef wctob
# if HAVE_RAW_DECL_WCTOB
_GL_WARN_ON_USE (wctob, "wctob is unportable - "
                 "use gnulib module wctob for portability");
# endif
#endif


/* Test whether *PS is in the initial state.  */
#if @GNULIB_MBSINIT@
# if @REPLACE_MBSINIT@
#  undef mbsinit
#  define mbsinit rpl_mbsinit
# endif
# if !@HAVE_MBSINIT@ || @REPLACE_MBSINIT@
extern int mbsinit (const mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbsinit
# if HAVE_RAW_DECL_MBSINIT
_GL_WARN_ON_USE (mbsinit, "mbsinit is unportable - "
                 "use gnulib module mbsinit for portability");
# endif
#endif


/* Convert a multibyte character to a wide character.  */
#if @GNULIB_MBRTOWC@
# if @REPLACE_MBRTOWC@
#  undef mbrtowc
#  define mbrtowc rpl_mbrtowc
# endif
# if !@HAVE_MBRTOWC@ || @REPLACE_MBRTOWC@
extern size_t mbrtowc (wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbrtowc
# if HAVE_RAW_DECL_MBRTOWC
_GL_WARN_ON_USE (mbrtowc, "mbrtowc is unportable - "
                 "use gnulib module mbrtowc for portability");
# endif
#endif


/* Recognize a multibyte character.  */
#if @GNULIB_MBRLEN@
# if @REPLACE_MBRLEN@
#  undef mbrlen
#  define mbrlen rpl_mbrlen
# endif
# if !@HAVE_MBRLEN@ || @REPLACE_MBRLEN@
extern size_t mbrlen (const char *s, size_t n, mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbrlen
# if HAVE_RAW_DECL_MBRLEN
_GL_WARN_ON_USE (mbrlen, "mbrlen is unportable - "
                 "use gnulib module mbrlen for portability");
# endif
#endif


/* Convert a string to a wide string.  */
#if @GNULIB_MBSRTOWCS@
# if @REPLACE_MBSRTOWCS@
#  undef mbsrtowcs
#  define mbsrtowcs rpl_mbsrtowcs
# endif
# if !@HAVE_MBSRTOWCS@ || @REPLACE_MBSRTOWCS@
extern size_t mbsrtowcs (wchar_t *dest, const char **srcp, size_t len, mbstate_t *ps)
     _GL_ARG_NONNULL ((2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbsrtowcs
# if HAVE_RAW_DECL_MBSRTOWCS
_GL_WARN_ON_USE (mbsrtowcs, "mbsrtowcs is unportable - "
                 "use gnulib module mbsrtowcs for portability");
# endif
#endif


/* Convert a string to a wide string.  */
#if @GNULIB_MBSNRTOWCS@
# if @REPLACE_MBSNRTOWCS@
#  undef mbsnrtowcs
#  define mbsnrtowcs rpl_mbsnrtowcs
# endif
# if !@HAVE_MBSNRTOWCS@ || @REPLACE_MBSNRTOWCS@
extern size_t mbsnrtowcs (wchar_t *dest, const char **srcp, size_t srclen, size_t len, mbstate_t *ps)
     _GL_ARG_NONNULL ((2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbsnrtowcs
# if HAVE_RAW_DECL_MBSNRTOWCS
_GL_WARN_ON_USE (mbsnrtowcs, "mbsnrtowcs is unportable - "
                 "use gnulib module mbsnrtowcs for portability");
# endif
#endif


/* Convert a wide character to a multibyte character.  */
#if @GNULIB_WCRTOMB@
# if @REPLACE_WCRTOMB@
#  undef wcrtomb
#  define wcrtomb rpl_wcrtomb
# endif
# if !@HAVE_WCRTOMB@ || @REPLACE_WCRTOMB@
extern size_t wcrtomb (char *s, wchar_t wc, mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef wcrtomb
# if HAVE_RAW_DECL_WCRTOMB
_GL_WARN_ON_USE (wcrtomb, "wcrtomb is unportable - "
                 "use gnulib module wcrtomb for portability");
# endif
#endif


/* Convert a wide string to a string.  */
#if @GNULIB_WCSRTOMBS@
# if @REPLACE_WCSRTOMBS@
#  undef wcsrtombs
#  define wcsrtombs rpl_wcsrtombs
# endif
# if !@HAVE_WCSRTOMBS@ || @REPLACE_WCSRTOMBS@
extern size_t wcsrtombs (char *dest, const wchar_t **srcp, size_t len, mbstate_t *ps)
     _GL_ARG_NONNULL ((2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef wcsrtombs
# if HAVE_RAW_DECL_WCSRTOMBS
_GL_WARN_ON_USE (wcsrtombs, "wcsrtombs is unportable - "
                 "use gnulib module wcsrtombs for portability");
# endif
#endif


/* Convert a wide string to a string.  */
#if @GNULIB_WCSNRTOMBS@
# if @REPLACE_WCSNRTOMBS@
#  undef wcsnrtombs
#  define wcsnrtombs rpl_wcsnrtombs
# endif
# if !@HAVE_WCSNRTOMBS@ || @REPLACE_WCSNRTOMBS@
extern size_t wcsnrtombs (char *dest, const wchar_t **srcp, size_t srclen, size_t len, mbstate_t *ps)
     _GL_ARG_NONNULL ((2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef wcsnrtombs
# if HAVE_RAW_DECL_WCSNRTOMBS
_GL_WARN_ON_USE (wcsnrtombs, "wcsnrtombs is unportable - "
                 "use gnulib module wcsnrtombs for portability");
# endif
#endif


/* Return the number of screen columns needed for WC.  */
#if @GNULIB_WCWIDTH@
# if @REPLACE_WCWIDTH@
#  undef wcwidth
#  define wcwidth rpl_wcwidth
extern int wcwidth (wchar_t);
# else
#  if !defined wcwidth && !@HAVE_DECL_WCWIDTH@
/* wcwidth exists but is not declared.  */
extern int wcwidth (int /* actually wchar_t */);
#  endif
# endif
#elif defined GNULIB_POSIXCHECK
# undef wcwidth
# if HAVE_RAW_DECL_WCWIDTH
_GL_WARN_ON_USE (wcwidth, "wcwidth is unportable - "
                 "use gnulib module wcwidth for portability");
# endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_WCHAR_H */
#endif /* _GL_WCHAR_H */
#endif
