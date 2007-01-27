# strstr.m4 serial 6
dnl Copyright (C) 2002-2003, 2005, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRSTR],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  dnl No known system has a strstr() function that works correctly in
  dnl multibyte locales. Therefore we use our version always.
  AC_LIBOBJ(strstr)
  REPLACE_STRSTR=1
  gl_PREREQ_STRSTR
])

# Prerequisites of lib/strstr.c.
AC_DEFUN([gl_PREREQ_STRSTR], [
  AC_REQUIRE([gl_FUNC_MBRTOWC])
  :
])
