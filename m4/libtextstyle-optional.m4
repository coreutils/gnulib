# libtextstyle-optional.m4 serial 1
dnl Copyright (C) 2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

dnl gl_LIBTEXTSTYLE_OPTIONAL
dnl Searches for an installed libtextstyle or uses the included source code
dnl parts that define only part of the API and does not do any styling.
dnl If found, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=yes and the LIBTEXTSTYLE
dnl and LTLIBTEXTSTYLE variables, and augments the CPPFLAGS variable, and
dnl #defines HAVE_LIBTEXTSTYLE to 1.
dnl Otherwise, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=no and LIBTEXTSTYLE and
dnl LTLIBTEXTSTYLE to empty.

AC_DEFUN([gl_LIBTEXTSTYLE_OPTIONAL],
[
  AC_REQUIRE([gl_LIBTEXTSTYLE])
  if test $HAVE_LIBTEXTSTYLE = yes; then
    TEXTSTYLE_H=
  else
    TEXTSTYLE_H=textstyle.h
    AC_REQUIRE([AC_C_INLINE])
    AC_CHECK_FUNCS_ONCE([tcdrain])
  fi
  AC_SUBST([TEXTSTYLE_H])
  AM_CONDITIONAL([GL_GENERATE_TEXTSTYLE_H], [test -n "$TEXTSTYLE_H"])
])
