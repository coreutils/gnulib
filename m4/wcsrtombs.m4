# wcsrtombs.m4
# serial 15
dnl Copyright (C) 2008-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_WCSRTOMBS],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN

  AC_CHECK_FUNCS_ONCE([wcsrtombs])
  if test $ac_cv_func_wcsrtombs = no; then
    HAVE_WCSRTOMBS=0
    AC_CHECK_DECLS([wcsrtombs],,, [[
      #include <wchar.h>
    ]])
    if test $ac_cv_have_decl_wcsrtombs = yes; then
      dnl On Minix 3.1.8, the system's <wchar.h> declares wcsrtombs() although
      dnl it does not have the function. Avoid a collision with gnulib's
      dnl replacement.
      REPLACE_WCSRTOMBS=1
    fi
  else
    if test $REPLACE_MBSTATE_T = 1; then
      REPLACE_WCSRTOMBS=1
    else
      gl_WCSRTOMBS_NULL
      case "$gl_cv_func_wcsrtombs_null" in
        *yes) ;;
        *) AC_DEFINE([WCSRTOMBS_NULL_ARG_BUG], [1],
             [Define if the wcsrtombs function has the NULL destination argument bug.])
           REPLACE_WCSRTOMBS=1 ;;
      esac
    fi
  fi
])

dnl Test whether the wcsrtombs implementation supports a NULL argument
dnl correctly. This is not the case on HP-UX 11.11: This implementation
dnl updates the source pointer also if the destination argument is NULL.
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
          # Guess no on HP-UX, mingw.
        hpux* | mingw* | windows*)
          gl_cv_func_wcsrtombs_null="guessing no" ;;
          # Guess yes on native Windows.
        mingw* | windows*)
          gl_cv_func_wcsrtombs_null="guessing yes" ;;
          # Guess yes otherwise.
        *)
          gl_cv_func_wcsrtombs_null="guessing yes" ;;
      esac
changequote([,])dnl
      if test $LOCALE_FR != none; then
        AC_RUN_IFELSE(
          [AC_LANG_SOURCE([[
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
int main ()
{
  int result = 0;
  if (setlocale (LC_ALL, "$LOCALE_FR") != NULL)
    {
      const char original[] = "B\374\337er";
      wchar_t input[10];

      if (mbstowcs (input, original, 10) == 5)
        {
          const wchar_t *src = input;
          size_t ret = wcsrtombs (NULL, &src, 3, NULL);
          if (ret != 5)
            result |= 1;
          if (src != input)
            result |= 2;
        }
    }
  return result;
}]])],
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
