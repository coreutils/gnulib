# locale_h.m4 serial 4
dnl Copyright (C) 2007, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_LOCALE_H],
[
  dnl Persuade glibc <locale.h> to define locale_t.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl If <stddef.h> is replaced, then <locale.h> must also be replaced.
  AC_REQUIRE([gl_STDDEF_H])

  AC_CACHE_CHECK([whether locale.h conforms to POSIX:2001],
    [gl_cv_header_locale_h_posix2001],
    [AC_TRY_COMPILE([#include <locale.h>
int x = LC_MESSAGES;], [],
       [gl_cv_header_locale_h_posix2001=yes],
       [gl_cv_header_locale_h_posix2001=no])])

  dnl Check for <xlocale.h>.
  AC_CHECK_HEADERS_ONCE([xlocale.h])
  if test $ac_cv_header_xlocale_h = yes; then
    HAVE_XLOCALE_H=1
    dnl Check whether use of locale_t requires inclusion of <xlocale.h>,
    dnl e.g. on MacOS X 10.5. If <locale.h> does not define locale_t by
    dnl itself, we assume that <xlocale.h> will do so.
    AC_CACHE_CHECK([whether locale.h defines locale_t],
      [gl_cv_header_locale_has_locale_t],
      [AC_TRY_COMPILE([#include <locale.h>
locale_t x;], [],
         [gl_cv_header_locale_has_locale_t=yes],
         [gl_cv_header_locale_has_locale_t=no])
      ])
    if test $gl_cv_header_locale_has_locale_t = yes; then
      gl_cv_header_locale_h_needs_xlocale_h=no
    else
      gl_cv_header_locale_h_needs_xlocale_h=yes
    fi
  else
    HAVE_XLOCALE_H=0
    gl_cv_header_locale_h_needs_xlocale_h=no
  fi
  AC_SUBST([HAVE_XLOCALE_H])

  if test -z "$STDDEF_H" \
     && test $gl_cv_header_locale_h_posix2001 = yes \
     && test $gl_cv_header_locale_h_needs_xlocale_h = no; then
    LOCALE_H=
  else
    gl_CHECK_NEXT_HEADERS([locale.h])
    LOCALE_H=locale.h
  fi
  AC_SUBST([LOCALE_H])
])
