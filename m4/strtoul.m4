# strtoul.m4 serial 6
dnl Copyright (C) 2002, 2006, 2009-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRTOUL],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS([strtoul])
  if test $ac_cv_func_strtoul = yes; then
    AC_CACHE_CHECK([whether strtoul works],
      [gl_cv_func_strtoul_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stdlib.h>]],
            [[int result = 0;
              char *term;
              /* This test fails on Minix and native Windows.  */
              {
                const char input[] = "0x";
                (void) strtoul (input, &term, 16);
                if (term != input + 1)
                  result |= 1;
              }
              return result;
            ]])
         ],
         [gl_cv_func_strtoul_works=yes],
         [gl_cv_func_strtoul_works=no],
         [case "$host_os" in
                    # Guess no on native Windows.
            mingw*) gl_cv_func_strtoul_works="guessing no" ;;
            *)      gl_cv_func_strtoul_works="$gl_cross_guess_normal" ;;
          esac
         ])
    ])
    case "$gl_cv_func_strtoul_works" in
      *yes) ;;
      *)    REPLACE_STRTOUL=1 ;;
    esac
  else
    HAVE_STRTOUL=0
  fi
])
