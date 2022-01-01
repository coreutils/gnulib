# libtextstyle-optional.m4 serial 4
dnl Copyright (C) 2019-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

dnl gl_LIBTEXTSTYLE_OPTIONAL([MINIMUM-VERSION])
dnl Searches for an installed libtextstyle with version >= MINIMUM-VERSION
dnl   MINIMUM-VERSION = 0.20      - the first release in 2019
dnl   MINIMUM-VERSION = 0.20.5    - adds hyperlink support and ostream_printf
dnl   MINIMUM-VERSION unspecified - the newest release
dnl or uses the included source code parts that define only part of the API
dnl and do not do any styling.
dnl If found, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=yes and the LIBTEXTSTYLE
dnl and LTLIBTEXTSTYLE variables, and augments the CPPFLAGS variable, and
dnl #defines HAVE_LIBTEXTSTYLE to 1.
dnl Otherwise, it sets and AC_SUBSTs HAVE_LIBTEXTSTYLE=no and LIBTEXTSTYLE and
dnl LTLIBTEXTSTYLE to empty.

AC_DEFUN([gl_LIBTEXTSTYLE_OPTIONAL],
[
  gl_LIBTEXTSTYLE([$1])
  if test $HAVE_LIBTEXTSTYLE = yes; then
    GL_GENERATE_TEXTSTYLE_H=false
  else
    GL_GENERATE_TEXTSTYLE_H=true
    AC_REQUIRE([AC_C_INLINE])
    AC_CHECK_FUNCS_ONCE([tcdrain])
  fi

  dnl Update the value of the TEXTSTYLE_H variable and the
  dnl GL_GENERATE_TEXTSTYLE_H conditional.
  dnl In most modules, the gl_CONDITIONAL_HEADER invocation belongs in the
  dnl module description, not the .m4 file. But here it's OK, because it does
  dnl not make sense to AC_REQUIRE([gl_LIBTEXTSTYLE_OPTIONAL]).
  gl_CONDITIONAL_HEADER([textstyle.h])
])
