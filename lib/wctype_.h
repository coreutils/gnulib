/* A substitute for ISO C99 <wctype.h>, for platforms that lack it.

   Copyright (C) 2006 Free Software Foundation, Inc.

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

/* Written by Bruno Haible and Paul Eggert.  */

/*
 * ISO C 99 <wctype.h> for platforms that lack it.
 * <http://www.opengroup.org/susv3xbd/wctype.h.html>
 *
 * iswctype, towctrans, towlower, towupper, wctrans, wctype,
 * wctrans_t, and wctype_t are not yet implemented.
 */

#ifndef _GL_WCTYPE_H
#define _GL_WCTYPE_H

#if @HAVE_WINT_T@ - 0
/* Solaris 2.5 has a bug: <wchar.h> must be included before <wctype.h>.
   Tru64 with Desktop Toolkit C has a bug: <stdio.h> must be included before
   <wchar.h>.
   BSD/OS 4.1 has a bug: <stdio.h> and <time.h> must be included before
   <wchar.h>.  */
# include <stdio.h>
# include <time.h>
# include <wchar.h>
typedef wint_t __wctype_wint_t;
#else
typedef int __wctype_wint_t;
#endif

/* Include the original <wctype.h> if it exists.
   BeOS 5 has the functions but no <wctype.h>.  */
#if @HAVE_WCTYPE_H@
# include @ABSOLUTE_WCTYPE_H@
#endif

/* IRIX 5.3 has a bug: its isw* macros reference an undefined variable
   _ctmp_.  */
#if @HAVE_WCTYPE_CTMP_BUG@
static wint_t _ctmp_;
#endif

/* FreeBSD 4.4 to 4.11 has <wctype.h> but lacks the functions.
   Assume all 12 functions are implemented the same way, or not at all.  */

#if !defined iswalnum && !HAVE_ISWCNTRL
static inline int
iswalnum (__wctype_wint_t wc)
{
  return ((wc >= '0' && wc <= '9')
	  || ((wc & ~0x20) >= 'A' && (wc & ~0x20) <= 'Z'));
}
# define iswalnum iswalnum
#endif

#if !defined iswalpha && !HAVE_ISWCNTRL
static inline int
iswalpha (__wctype_wint_t wc)
{
  return (wc & ~0x20) >= 'A' && (wc & ~0x20) <= 'Z';
}
# define iswalpha iswalpha
#endif

#if !defined iswblank && !HAVE_ISWCNTRL
static inline int
iswblank (__wctype_wint_t wc)
{
  return wc == ' ' || wc == '\t';
}
# define iswblank iswblank
#endif

#if !defined iswcntrl && !HAVE_ISWCNTRL
static inline int
iswcntrl (__wctype_wint_t wc)
{
  return (wc & ~0x1f) == 0 || wc == 0x7f;
}
# define iswcntrl iswcntrl
#endif

#if !defined iswdigit && !HAVE_ISWCNTRL
static inline int
iswdigit (__wctype_wint_t wc)
{
  return wc >= '0' && wc <= '9';
}
# define iswdigit iswdigit
#endif

#if !defined iswgraph && !HAVE_ISWCNTRL
static inline int
iswgraph (__wctype_wint_t wc)
{
  return wc >= '!' && wc <= '~';
}
# define iswgraph iswgraph
#endif

#if !defined iswlower && !HAVE_ISWCNTRL
static inline int
iswlower (__wctype_wint_t wc)
{
  return wc >= 'a' && wc <= 'z';
}
# define iswlower iswlower
#endif

#if !defined iswprint && !HAVE_ISWCNTRL
static inline int
iswprint (__wctype_wint_t wc)
{
  return wc >= ' ' && wc <= '~';
}
# define iswprint iswprint
#endif

#if !defined iswpunct && !HAVE_ISWCNTRL
static inline int
iswpunct (__wctype_wint_t wc)
{
  return (wc >= '!' && wc <= '~'
	  && !((wc >= '0' && wc <= '9')
	       || ((wc & ~0x20) >= 'A' && (wc & ~0x20) <= 'Z')));
}
# define iswpunct iswpunct
#endif

#if !defined iswspace && !HAVE_ISWCNTRL
static inline int
iswspace (__wctype_wint_t wc)
{
  return (wc == ' ' || wc == '\t'
	  || wc == '\n' || wc == '\v' || wc == '\f' || wc == '\r');
}
# define iswspace iswspace
#endif

#if !defined iswupper && !HAVE_ISWCNTRL
static inline int
iswupper (__wctype_wint_t wc)
{
  return wc >= 'A' && wc <= 'Z';
}
# define iswupper iswupper
#endif

#if !defined iswxdigit && !HAVE_ISWCNTRL
static inline int
iswxdigit (__wctype_wint_t wc)
{
  return ((wc >= '0' && wc <= '9')
	  || ((wc & ~0x20) >= 'A' && (wc & ~0x20) <= 'F'));
}
# define iswxdigit iswxdigit
#endif


#endif /* _GL_WCTYPE_H */
