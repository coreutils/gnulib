# c32rtomb.m4 serial 1
dnl Copyright (C) 2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_C32RTOMB],
[
  AC_REQUIRE([gl_UCHAR_H_DEFAULTS])

  AC_REQUIRE([gl_MBRTOC32_SANITYCHECK])

  AC_CHECK_FUNCS_ONCE([c32rtomb])
  if test $ac_cv_func_c32rtomb = no; then
    HAVE_C32RTOMB=0
  else
    dnl When we override mbrtoc32, redefining the meaning of the char32_t
    dnl values, we need to override c32rtomb as well, for consistency.
    if test $HAVE_WORKING_MBRTOC32 = 0; then
      REPLACE_C32RTOMB=1
    fi
    AC_CACHE_CHECK([whether c32rtomb return value is correct],
      [gl_cv_func_c32rtomb_retval],
      [
        dnl Initial guess, used when cross-compiling.
changequote(,)dnl
        case "$host_os" in
          # Guess no on AIX.
          aix*) gl_cv_func_c32rtomb_retval="guessing no" ;;
          # Guess yes otherwise.
          *)    gl_cv_func_c32rtomb_retval="guessing yes" ;;
        esac
changequote([,])dnl
        AC_RUN_IFELSE(
          [AC_LANG_SOURCE([[
#include <uchar.h>
int main ()
{
  int result = 0;
  if (c32rtomb (NULL, 0, NULL) != 1)
    result |= 1;
  return result;
}]])],
          [gl_cv_func_c32rtomb_retval=yes],
          [gl_cv_func_c32rtomb_retval=no],
          [:])
      ])
    case "$gl_cv_func_c32rtomb_retval" in
      *yes) ;;
      *) AC_DEFINE([C32RTOMB_RETVAL_BUG], [1],
           [Define if the wcrtomb function has an incorrect return value.])
         REPLACE_C32RTOMB=1 ;;
    esac
  fi
])
