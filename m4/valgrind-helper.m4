# valgrind-helper.m4
# serial 3
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Enables support for running the generated binaries under valgrind if
# - the valgrind header files are installed, and
# - the user desires so.
AC_DEFUN_ONCE([gl_VALGRIND_HELPER],
[
  AC_ARG_WITH([valgrind],
    [AS_HELP_STRING([[--with-valgrind]],
       [enable support for running the binaries under valgrind])],
    [if test "$withval" != no; then
       support_valgrind=1
     else
       support_valgrind=0
     fi
    ],
    [support_valgrind=0])
  if test $support_valgrind = 1; then
    AC_CHECK_HEADERS([valgrind/valgrind.h])
    if test $ac_cv_header_valgrind_valgrind_h != yes; then
      AC_MSG_ERROR([cannot enable valgrind support: <valgrind/valgrind.h> not found])
    fi
  fi
  AC_DEFINE_UNQUOTED([ENABLE_VALGRIND_SUPPORT], [$support_valgrind],
    [Define to 1 to include support for running the binaries under valgrind,
     or to 0 otherwise.])
])
