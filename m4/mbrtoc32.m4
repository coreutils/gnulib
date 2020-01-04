# mbrtoc32.m4 serial 1
dnl Copyright (C) 2014-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MBRTOC32],
[
  AC_REQUIRE([gl_UCHAR_H_DEFAULTS])

  AC_REQUIRE([AC_TYPE_MBSTATE_T])
  gl_MBSTATE_T_BROKEN

  AC_CHECK_FUNCS_ONCE([mbrtoc32])
  if test $ac_cv_func_mbrtoc32 = no; then
    HAVE_MBRTOC32=0
  else
    if test $REPLACE_MBSTATE_T = 1; then
      REPLACE_MBRTOC32=1
    else
      gl_MBRTOC32_EMPTY_INPUT
      gl_MBRTOC32_C_LOCALE
      case "$gl_cv_func_mbrtoc32_empty_input" in
        *yes) ;;
        *) AC_DEFINE([MBRTOC32_EMPTY_INPUT_BUG], [1],
             [Define if the mbrtoc32 function does not return (size_t) -2 for empty input.])
           REPLACE_MBRTOC32=1
           ;;
      esac
      case "$gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ" in
        *yes) ;;
        *) AC_DEFINE([MBRTOC32_IN_C_LOCALE_MAYBE_EILSEQ], [1],
             [Define if the mbrtoc32 function may signal encoding errors in the C locale.])
           REPLACE_MBRTOC32=1
           ;;
      esac
    fi
  fi
])

AC_DEFUN([gl_MBRTOC32_EMPTY_INPUT],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether mbrtoc32 works on empty input],
    [gl_cv_func_mbrtoc32_empty_input],
    [
      dnl Initial guess, used when cross-compiling or when no suitable locale
      dnl is present.
changequote(,)dnl
      case "$host_os" in
                       # Guess no on glibc systems.
        *-gnu* | gnu*) gl_cv_func_mbrtoc32_empty_input="guessing no" ;;
        *)             gl_cv_func_mbrtoc32_empty_input="guessing yes" ;;
      esac
changequote([,])dnl
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
           #include <uchar.h>
           static char32_t wc;
           static mbstate_t mbs;
           int
           main (void)
           {
             return mbrtoc32 (&wc, "", 0, &mbs) != (size_t) -2;
           }]])],
        [gl_cv_func_mbrtoc32_empty_input=yes],
        [gl_cv_func_mbrtoc32_empty_input=no],
        [:])
    ])
])

AC_DEFUN([gl_MBRTOC32_C_LOCALE],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether the C locale is free of encoding errors],
    [gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ],
    [
     dnl Initial guess, used when cross-compiling or when no suitable locale
     dnl is present.
     gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ="$gl_cross_guess_normal"

     AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <limits.h>
            #include <locale.h>
            #include <uchar.h>
          ]], [[
            int i;
            char *locale = setlocale (LC_ALL, "C");
            if (! locale)
              return 2;
            for (i = CHAR_MIN; i <= CHAR_MAX; i++)
              {
                char c = i;
                char32_t wc;
                mbstate_t mbs = { 0, };
                size_t ss = mbrtoc32 (&wc, &c, 1, &mbs);
                if (1 < ss)
                  return 3;
              }
            return 0;
          ]])],
      [gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ=yes],
      [gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ=no],
      [case "$host_os" in
                 # Guess yes on native Windows.
         mingw*) gl_cv_func_mbrtoc32_C_locale_sans_EILSEQ="guessing yes" ;;
       esac
      ])
    ])
])

# Prerequisites of lib/mbrtoc32.c and lib/lc-charset-dispatch.c.
AC_DEFUN([gl_PREREQ_MBRTOC32], [
  :
])
