# wcsrtombs.m4 serial 5
dnl Copyright (C) 2008-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCSRTOMBS],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN

  AC_CHECK_FUNCS_ONCE([wcsrtombs])
  if test $ac_cv_func_wcsrtombs = no; then
    HAVE_WCSRTOMBS=0
  else
    if test $REPLACE_MBSTATE_T = 1; then
      REPLACE_WCSRTOMBS=1
    else
      gl_WCSRTOMBS_TERMINATION
      gl_WCSRTOMBS_NULL
      case "$gl_cv_func_wcsrtombs_termination" in
        *yes) ;;
        *) AC_DEFINE([WCSRTOMBS_TERMINATION_BUG], [1],
             [Define if the wcsrtombs function may set the source pointer to NULL without NUL-terminating the destination.])
           REPLACE_WCSRTOMBS=1 ;;
      esac
      case "$gl_cv_func_wcsrtombs_null" in
        *yes) ;;
        *) AC_DEFINE([WCSRTOMBS_NULL_ARG_BUG], [1],
             [Define if the wcsrtombs function has the NULL destination argument bug.])
           REPLACE_WCSRTOMBS=1 ;;
      esac
    fi
  fi
  if test $HAVE_WCSRTOMBS = 0 || test $REPLACE_WCSRTOMBS = 1; then
    gl_REPLACE_WCHAR_H
    AC_LIBOBJ([wcsrtombs])
    AC_LIBOBJ([wcsrtombs-state])
    gl_PREREQ_WCSRTOMBS
  fi
])

dnl Test whether the wcsrtombs implementation stores a non-NULL source pointer
dnl as long as it has not written the final NUL byte to the destination string.
dnl The OSF/1 5.1 implementation stores a NULL pointer already after storing
dnl the last non-NUL character, even if there is no more room for the
dnl terminating NUL character.
dnl Result is gl_cv_func_wcsrtombs_termination.

AC_DEFUN([gl_WCSRTOMBS_TERMINATION],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gt_LOCALE_FR])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether wcsrtombs updates the source pointer correctly],
    [gl_cv_func_wcsrtombs_termination],
    [
      dnl Initial guess, used when cross-compiling or when no suitable locale
      dnl is present.
changequote(,)dnl
      case "$host_os" in
              # Guess no on OSF/1.
        osf*) gl_cv_func_wcsrtombs_termination="guessing no" ;;
              # Guess yes otherwise.
        *)    gl_cv_func_wcsrtombs_termination="guessing yes" ;;
      esac
changequote([,])dnl
      if test $LOCALE_FR != none; then
        AC_TRY_RUN([
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
int main ()
{
  if (setlocale (LC_ALL, "$LOCALE_FR") != NULL)
    {
      const char original[] = "B\374\337er";
      wchar_t input[10];

      if (mbstowcs (input, original, 10) == 5)
        {
          const wchar_t *src = input;
          char output[5];
          wcsrtombs (output, &src, 5, NULL);
          if (src != input + 5)
            return 1;
        }
    }
  return 0;
}],
          [gl_cv_func_wcsrtombs_termination=yes],
          [gl_cv_func_wcsrtombs_termination=no],
          [:])
      fi
    ])
])

dnl Test whether the wcsrtombs implementation supports a NULL argument
dnl correctly. This is not the case on HP-UX 11.11 and OSF/1 5.1: These
dnl implementations updates the source pointer also if the destination argument
dnl is NULL.
dnl Result is gl_cv_func_wcsrtombs_null.

AC_DEFUN([gl_WCSRTOMBS_NULL],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gt_LOCALE_FR])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether wcsrtombs supports a NULL argument correctly],
    [gl_cv_func_wcsrtombs_null],
    [
      dnl Initial guess, used when cross-compiling or when no suitable locale
      dnl is present.
changequote(,)dnl
      case "$host_os" in
                      # Guess no on HP-UX and OSF/1.
        hpux* | osf*) gl_cv_func_wcsrtombs_null="guessing no" ;;
                      # Guess yes otherwise.
        *)            gl_cv_func_wcsrtombs_null="guessing yes" ;;
      esac
changequote([,])dnl
      if test $LOCALE_FR != none; then
        AC_TRY_RUN([
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
int main ()
{
  if (setlocale (LC_ALL, "$LOCALE_FR") != NULL)
    {
      const char original[] = "B\374\337er";
      wchar_t input[10];

      if (mbstowcs (input, original, 10) == 5)
        {
          const wchar_t *src = input;
          wcsrtombs (NULL, &src, 10, NULL);
          if (src != input)
            return 1;
        }
    }
  return 0;
}],
          [gl_cv_func_wcsrtombs_null=yes],
          [gl_cv_func_wcsrtombs_null=no],
          [:])
      fi
    ])
])

# Prerequisites of lib/wcsrtombs.c.
AC_DEFUN([gl_PREREQ_WCSRTOMBS], [
  :
])
