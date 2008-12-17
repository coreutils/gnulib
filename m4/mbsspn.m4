# mbsspn.m4 serial 2
dnl Copyright (C) 2007-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBSSPN],
[
  gl_PREREQ_MBSSPN
])

# Prerequisites of lib/mbsspn.c.
AC_DEFUN([gl_PREREQ_MBSSPN], [
  AC_REQUIRE([AC_FUNC_MBRTOWC])
  :
])
