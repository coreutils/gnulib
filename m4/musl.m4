# musl.m4 serial 2
dnl Copyright (C) 2019-2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Test for musl libc, despite the musl libc authors don't like it
# <https://wiki.musl-libc.org/faq.html>
# <https://lists.gnu.org/archive/html/bug-gnulib/2018-02/msg00079.html>.
# From Bruno Haible.

# There are three ways to test for musl libc:
# a. Look at the 'ldd --version' output. This is how config.guess
#    does it. But it fails on Alpine Linux 3.10, because on this system
#    'ldd' is a shell script that does not understand the '--version'
#    option.
# b. Test whether the header file <bits/alltypes.h> exists.
# c. Test whether the header file <stdarg.h> defines the macro
#    __DEFINED_va_list.
# We use a+c.

AC_DEFUN_ONCE([gl_MUSL_LIBC],
[
  AC_REQUIRE([gl_MUSL_CANONICAL_HOST])
  case "$host_os" in
    *-musl*) AC_DEFINE([MUSL_LIBC], [1], [Define to 1 on musl libc.]) ;;
  esac
])

# Like AC_CANONICAL_HOST, except for a fix regarding Alpine Linux 3.10.

AC_DEFUN([gl_MUSL_CANONICAL_HOST],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  case "$host_os" in
    linux*-gnu*)
      AC_EGREP_CPP([actually_musl],
        [#include <stdarg.h>
         #ifdef __DEFINED_va_list
          actually_musl
         #endif
        ],
        [host=`echo "$host" | sed -e 's/gnu.*/musl/'`
         host_os=`echo "$host_os" | sed -e 's/gnu.*/musl/'`
        ])
      ;;
  esac
])
