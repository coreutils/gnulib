# getrandom.m4 serial 3
dnl Copyright 2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_FUNC_GETRANDOM],
[
  AC_REQUIRE([gl_SYS_RANDOM_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([getrandom])
  if test "$ac_cv_func_getrandom" != yes; then
    HAVE_GETRANDOM=0
  else
    dnl On Solaris 11.4 the return type is 'int', not 'ssize_t'.
    AC_CACHE_CHECK([whether getrandom is compatible with its GNU+BSD signature],
      [gl_cv_func_getrandom_ok],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <sys/random.h>
              #include <sys/types.h>
              ssize_t getrandom (void *, size_t, unsigned int);
            ]],
            [[]])
         ],
         [gl_cv_func_getrandom_ok=yes],
         [gl_cv_func_getrandom_ok=no])
      ])
    if test $gl_cv_func_getrandom_ok = no; then
      REPLACE_GETRANDOM=1
    fi
  fi
])
