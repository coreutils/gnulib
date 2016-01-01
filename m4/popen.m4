# popen.m4 serial 5
dnl Copyright (C) 2009-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_POPEN],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_CHECK_FUNCS_ONCE([popen])
  if test $ac_cv_func_popen = no; then
    HAVE_POPEN=0
  else
    AC_CACHE_CHECK([whether popen works with closed stdin],
      [gl_cv_func_popen_works],
      [
        AC_RUN_IFELSE(
          [AC_LANG_PROGRAM(
             [[#include <stdio.h>]],
             [[int result = 0;
               FILE *child;
               fclose (stdin);
               fclose (stdout);
               child = popen ("echo a", "r");
               if (fgetc (child) != 'a')
                 result |= 1;
               if (pclose (child) != 0)
                 result |= 2;
               return result;
             ]])],
          [gl_cv_func_popen_works=yes],
          [gl_cv_func_popen_works=no],
          dnl For now, only cygwin 1.5 or older is known to be broken.
          [gl_cv_func_popen_works='guessing yes'])
      ])
    if test "$gl_cv_func_popen_works" = no; then
      REPLACE_POPEN=1
    fi
  fi
])

# Prerequisites of lib/popen.c.
AC_DEFUN([gl_PREREQ_POPEN],
[
  :
])
