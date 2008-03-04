# __func__.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Simon Josefsson

AC_DEFUN([gl___FUNC__],
[
  AC_CACHE_CHECK([whether __func__ is available], [gl_cv_var___func__],
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[]], [[const char *str = __func__;]])],
       [gl_cv_var___func__=yes],
       [gl_cv_var___func__=no]))
  if test "$gl_cv_var___func__" != yes; then
    AC_DEFINE([__func__], ["<unknown function>"],
              [Define as a replacement for the ISO C99 __func__ variable.])
  fi
])
