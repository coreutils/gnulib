# strcasestr.m4 serial 3
dnl Copyright (C) 2005, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRCASESTR],
[
  dnl No known system has a strcasestr() function that works correctly in
  dnl multibyte locales. Therefore we use our version always.
  AC_LIBOBJ(strcasestr)
  REPLACE_STRCASESTR=1
  gl_PREREQ_STRCASESTR
])

# Prerequisites of lib/strcasestr.c.
AC_DEFUN([gl_PREREQ_STRCASESTR], [
  AC_REQUIRE([gl_FUNC_MBRTOWC])
  :
])
