# wcsstr.m4
# serial 4
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Check that wcsstr exists and works.
AC_DEFUN([gl_FUNC_WCSSTR_SIMPLE],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([wcsstr])
  if test $ac_cv_func_wcsstr = no; then
    HAVE_WCSSTR=0
  fi
])

dnl Additionally, check that wcsstr is efficient.
AC_DEFUN([gl_FUNC_WCSSTR],
[
  AC_REQUIRE([gl_FUNC_WCSSTR_SIMPLE])
  if test $HAVE_WCSSTR = 1; then
    AC_CACHE_CHECK([whether wcsstr works in linear time],
      [gl_cv_func_wcsstr_linear],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <signal.h> /* for signal */
#include <wchar.h> /* for wcsstr */
#include <stdlib.h> /* for malloc */
#include <unistd.h> /* for alarm */
static void quit (int sig) { _exit (sig + 128); }
]], [[
    int result = 0;
    size_t m = 1000000;
    wchar_t *haystack = (wchar_t *) malloc ((2 * m + 2) * sizeof (wchar_t));
    wchar_t *needle = (wchar_t *) malloc ((m + 2) * sizeof (wchar_t));
    /* Failure to compile this test due to missing alarm is okay,
       since all such platforms (mingw, MSVC) also have quadratic wcsstr.  */
    signal (SIGALRM, quit);
    alarm (5);
    /* Check for quadratic performance.  */
    if (haystack && needle)
      {
        size_t i;
        for (i = 0; i < 2 * m; i++)
          haystack[i] = L'A';
        haystack[2 * m] = L'B';
        haystack[2 * m + 1] = 0;
        for (i = 0; i < m; i++)
          needle[i] = L'A';
        needle[m] = L'B';
        needle[m + 1] = 0;
        if (!wcsstr (haystack, needle))
          result |= 1;
      }
    /* Free allocated memory, in case some sanitizer is watching.  */
    free (haystack);
    free (needle);
    return result;
    ]])],
        [gl_cv_func_wcsstr_linear=yes], [gl_cv_func_wcsstr_linear=no],
        [gl_cv_func_wcsstr_linear="$gl_cross_guess_normal"])
      ])
    case "$gl_cv_func_wcsstr_linear" in
      *yes) ;;
      *)
        REPLACE_WCSSTR=1
        ;;
    esac
  fi
  if test $HAVE_WCSSTR = 0 || test $REPLACE_WCSSTR = 1; then
    AC_DEFINE([NEED_LINEAR_WCSSTR], [1],
      [Define to 1 to get a worst-case linear time implementation of wcsstr.])
  fi
])
