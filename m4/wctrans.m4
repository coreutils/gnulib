# wctrans.m4
# serial 4
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_FUNC_WCTRANS],
[
  AC_REQUIRE([gl_WCTYPE_H_DEFAULTS])
  AC_REQUIRE([gl_WCTYPE_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  HAVE_WCTRANS=$HAVE_WCTRANS_T
  if test $HAVE_WCTRANS = 1; then
    AC_CACHE_CHECK([whether wctrans works],
      [gl_cv_func_wctrans_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
            #include <wchar.h>
            #include <wctype.h>
            int main ()
            {
              /* This test fails on NetBSD 10.0.  */
              return (wctrans ("tolower") == (wctrans_t)0);
            }
         ]])],
         [gl_cv_func_wctrans_works=yes], [gl_cv_func_wctrans_works=no],
         [case "$host_os" in
                     # Guess no on NetBSD.
            netbsd*) gl_cv_func_wctrans_works="guessing no" ;;
                     # Guess yes otherwise.
            *)       gl_cv_func_wctrans_works="guessing yes" ;;
          esac
         ])
      ])
    case "$gl_cv_func_wctrans_works" in
      *yes) ;;
      *) REPLACE_WCTRANS=1 ;;
    esac
  fi
])
