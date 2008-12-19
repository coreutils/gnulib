/* A substitute for ISO C99 <wchar.h>, for platforms that have issues.

   Copyright (C) 2007-2008 Free Software Foundation, Inc.

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

#ifdef __need_mbstate_t
/* Special invocation convention inside uClibc header files.  */

#@INCLUDE_NEXT@ @NEXT_WCHAR_H@

#else
/* Normal invocation convention.  */

#ifndef _GL_WCHAR_H

/* Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.0.1 has a bug: <stddef.h>, <stdio.h> and <time.h> must be
   included before <wchar.h>.  */
#include <stddef.h>
#include <stdio.h>
#include <time.h>

/* Include the original <wchar.h> if it exists.
   Some builds of uClibc lack it.  */
/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_WCHAR_H@
# @INCLUDE_NEXT@ @NEXT_WCHAR_H@
#endif

#ifndef _GL_WCHAR_H
#define _GL_WCHAR_H

/* The definition of GL_LINK_WARNING is copied here.  */

#ifdef __cplusplus
extern "C" {
#endif


/* Define wint_t.  (Also done in wctype.in.h.)  */
#if !@HAVE_WINT_T@ && !defined wint_t
# define wint_t int
#endif


/* Override mbstate_t if it is too small.
   On IRIX 6.5, sizeof (mbstate_t) == 1, which is not sufficient for
   implementing mbrtowc for encodings like UTF-8.  */
#if !(@HAVE_MBSINIT@ && @HAVE_MBRTOWC@)
typedef int rpl_mbstate_t;
# undef mbstate_t
# define mbstate_t rpl_mbstate_t
#endif


/* Convert a single-byte character to a wide character.  */
#if @GNULIB_BTOWC@
# if !@HAVE_BTOWC@
extern wint_t btowc (int c);
# endif
#elif defined GNULIB_POSIXCHECK
# undef btowc
# define btowc(c) \
    (GL_LINK_WARNING ("btowc is unportable - " \
                      "use gnulib module btowc for portability"), \
     btowc (c))
#endif


/* Convert a wide character to a single-byte character.  */
#if @GNULIB_WCTOB@
# if !@HAVE_WCTOB@
extern int wctob (wint_t wc);
# endif
#elif defined GNULIB_POSIXCHECK
# undef wctob
# define wctob(w) \
    (GL_LINK_WARNING ("wctob is unportable - " \
                      "use gnulib module wctob for portability"), \
     wctob (w))
#endif


/* Test whether *PS is in the initial state.  */
#if @GNULIB_MBSINIT@
# if !@HAVE_MBSINIT@
extern int mbsinit (const mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbsinit
# define mbsinit(p) \
    (GL_LINK_WARNING ("mbsinit is unportable - " \
                      "use gnulib module mbsinit for portability"), \
     mbsinit (p))
#endif


/* Convert a multibyte character to a wide character.  */
#if @GNULIB_MBRTOWC@
# if !@HAVE_MBRTOWC@
extern size_t mbrtowc (wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbrtowc
# define mbrtowc(w,s,n,p) \
    (GL_LINK_WARNING ("mbrtowc is unportable - " \
                      "use gnulib module mbrtowc for portability"), \
     mbrtowc (w, s, n, p))
#endif


/* Recognize a multibyte character.  */
#if @GNULIB_MBRLEN@
# if !@HAVE_MBRLEN@
extern size_t mbrlen (const char *s, size_t n, mbstate_t *ps);
# endif
#elif defined GNULIB_POSIXCHECK
# undef mbrlen
# define mbrlen(s,n,p) \
    (GL_LINK_WARNING ("mbrlen is unportable - " \
                      "use gnulib module mbrlen for portability"), \
     mbrlen (s, n, p))
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
# define wcwidth(w) \
    (GL_LINK_WARNING ("wcwidth is unportable - " \
                      "use gnulib module wcwidth for portability"), \
     wcwidth (w))
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_WCHAR_H */
#endif /* _GL_WCHAR_H */
#endif
