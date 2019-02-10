# libtextstyle.m4 serial 1
dnl Copyright (C) 2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

dnl gl_LIBTEXTSTYLE
dnl Searches for an installed libtextstyle.
dnl If found, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=yes and the LIBTEXTSTYLE
dnl and LTLIBTEXTSTYLE variables, and augments the CPPFLAGS variable, and
dnl #defines HAVE_LIBTEXTSTYLE to 1.
dnl Otherwise, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=no and LIBTEXTSTYLE and
dnl LTLIBTEXTSTYLE to empty.

AC_DEFUN([gl_LIBTEXTSTYLE],
[
  AC_LIB_HAVE_LINKFLAGS([textstyle], [],
    [#include <textstyle.h>], [term_styled_ostream_create(1,"",TTYCTL_AUTO,"");],
    [no])
])
