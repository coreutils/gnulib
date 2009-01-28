# pmccabe2html.m4 serial 2
dnl Copyright (C) 2008, 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Simon Josefsson

# Usage: gl_PMCCABE2HTML([]).
AC_DEFUN([gl_PMCCABE2HTML],
[
  AC_REQUIRE([AC_PROG_AWK])
  AC_PATH_PROG([PMCCABE], [pmccabe], [false])
])
