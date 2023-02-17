# ffsll.m4 serial 5
dnl Copyright (C) 2011-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FFSLL],
[
  AC_REQUIRE([gl_STRING_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade glibc <string.h> and AIX <strings.h> to declare ffsll().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  dnl We can't use AC_CHECK_FUNC here, because ffsll() is defined as a
  dnl static inline function when compiling for Android 13 or older.
  dnl But require that ffsll() is declared; otherwise we may be using
  dnl the GCC built-in function, which leads to warnings
  dnl "warning: implicit declaration of function 'ffsll'".
  AC_CACHE_CHECK([for ffsll], [gl_cv_func_ffsll],
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <string.h>
            #include <strings.h>
            long long x;
          ]],
          [[int (*func) (long long) = ffsll;
            return func (x);
          ]])
       ],
       [gl_cv_func_ffsll=yes],
       [gl_cv_func_ffsll=no])
    ])
  if test $gl_cv_func_ffsll = yes; then
    dnl Test whether ffsll works.
    dnl On AIX 7.2 in 32-bit mode it is completely broken.
    AC_CACHE_CHECK([whether ffsll works],
      [gl_cv_func_ffsll_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
            #include <string.h>
            #include <strings.h>
            int dummy (long long x) { return 42; }
            int main (int argc, char *argv[])
            {
              int (* volatile my_ffsll) (long long) = argc ? ffsll : dummy;
              long long int x = -128LL;
              return my_ffsll (x) != 8;
            }
          ]])],
         [gl_cv_func_ffsll_works=yes],
         [gl_cv_func_ffsll_works=no],
         [case "$host_os" in
                                # Guess yes on glibc systems.
            *-gnu* | gnu*)      gl_cv_func_ffsll_works="guessing yes" ;;
                                # Guess yes on musl systems.
            *-musl* | midipix*) gl_cv_func_ffsll_works="guessing yes" ;;
                                # Guess yes on native Windows.
            mingw*)             gl_cv_func_ffsll_works="guessing yes" ;;
                                # Guess no on AIX.
            aix*)               gl_cv_func_ffsll_works="guessing no" ;;
                                # If we don't know, obey --enable-cross-guesses.
            *)                  gl_cv_func_ffsll_works="$gl_cross_guess_normal" ;;
          esac
         ])
      ])
    case "$gl_cv_func_ffsll_works" in
      *yes) ;;
      *) REPLACE_FFSLL=1 ;;
    esac
  else
    HAVE_FFSLL=0
  fi
])
