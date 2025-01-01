# mbstowcs.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_MBSTOWCS],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])

  gl_MBRTOWC_C_LOCALE
  case "$gl_cv_func_mbrtowc_C_locale_sans_EILSEQ" in
    *yes) ;;
    *) REPLACE_MBSTOWCS=1 ;;
  esac
])

# Prerequisites of lib/mbstowcs.c.
AC_DEFUN([gl_PREREQ_MBSTOWCS], [
  :
])
