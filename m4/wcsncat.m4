# wcsncat.m4
# serial 4
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_WCSNCAT],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  AC_CHECK_FUNCS_ONCE([wcsncat])
  if test $ac_cv_func_wcsncat = yes; then
    AC_CACHE_CHECK([for working wcsncat], [gl_cv_func_wcsncat_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <wchar.h>
              #include <stddef.h>
            ]], [[
              wchar_t y[1];
              int volatile value = (wcsncat (y, NULL, 0) == y);
              return !value;
            ]])
         ],
         [gl_cv_func_wcsncat_works=yes],
         [gl_cv_func_wcsncat_works=no],
         [case "$host_os" in
            # Guess no on macOS, NetBSD, OpenBSD, Solaris, Cygwin.
            darwin* | netbsd* | openbsd* | solaris* | cygwin*)
              gl_cv_func_wcsncat_works="guessing no" ;;
            *)
              gl_cv_func_wcsncat_works="guessing yes" ;;
          esac
         ])
      ])
    case "$gl_cv_func_wcsncat_works" in
      *yes) ;;
      *) REPLACE_WCSNCAT=1 ;;
    esac
  else
    HAVE_WCSNCAT=0
  fi
])
