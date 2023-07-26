# wctype.m4 serial 3
dnl Copyright (C) 2011-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN_ONCE([gl_FUNC_WCTYPE],
[
  AC_REQUIRE([gl_WCTYPE_H_DEFAULTS])
  AC_REQUIRE([gl_WCTYPE_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  HAVE_WCTYPE=$HAVE_WCTYPE_T
  if test $HAVE_WCTYPE = 1; then
    AC_CACHE_CHECK([whether wctype supports the "blank" character class],
      [gl_cv_func_wctype_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
            #include <wchar.h>
            #include <wctype.h>
            int main () { return wctype ("blank") == (wctype_t)0; }
         ]])],
         [gl_cv_func_wctype_works=yes], [gl_cv_func_wctype_works=no],
         [case "$host_os" in
                    # Guess no on mingw.
            mingw*) AC_EGREP_CPP([Problem], [
#ifdef __MINGW32__
 Problem
#endif
                      ],
                      [gl_cv_func_wctype_works="guessing no"],
                      [gl_cv_func_wctype_works="guessing yes"])
                    ;;
                    # Guess yes otherwise.
            *)      gl_cv_func_wctype_works="guessing yes" ;;
          esac
         ])
      ])
    case "$gl_cv_func_wctype_works" in
      *yes) ;;
      *) REPLACE_WCTYPE=1 ;;
    esac
  fi
])
