# dup.m4 serial 1
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_DUP],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_HEADERS_ONCE([unistd.h])
  AC_CACHE_CHECK([whether dup supports bad arguments], [gl_cv_func_dup_works],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM([[
#if HAVE_UNISTD_H
# include <unistd.h>
#else
# include <io.h>
#endif
#include <errno.h>
]],
          [[if (dup (-1) != -1)
              return 1;
            if (errno != EBADF)
              return 2;
            return 0;
          ]])],
       [gl_cv_func_dup_works=yes],
       [gl_cv_func_dup_works=no],
       [case "$host_os" in
          mingw*) gl_cv_func_dup_works="guessing no" ;;
          *)      gl_cv_func_dup_works="guessing yes" ;;
        esac
       ])
    ])
  case "$gl_cv_func_dup_works" in
    *no) REPLACE_DUP=1 ;;
  esac
  dnl Replace dup() for supporting the gnulib-defined fchdir() function,
  dnl to keep fchdir's bookkeeping up-to-date.
  m4_ifdef([gl_FUNC_FCHDIR], [
    gl_TEST_FCHDIR
    if test $HAVE_FCHDIR = 0; then
      REPLACE_DUP=1
    fi
  ])
])

# Prerequisites of lib/dup.c.
AC_DEFUN([gl_PREREQ_DUP], [
  AC_REQUIRE([AC_C_INLINE])
])
