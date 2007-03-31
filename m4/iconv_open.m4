# iconv_open.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ICONV_OPEN],
[
  AC_REQUIRE([AM_ICONV])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([gl_ICONV_H_DEFAULTS])
  if test "$am_cv_func_iconv" = yes; then
    dnl Test whether iconv_open accepts standardized encoding names.
    dnl We know that GNU libiconv and GNU libc do.
    AC_EGREP_CPP([gnu_iconv], [
      #include <iconv.h>
      #if defined _LIBICONV_VERSION || defined __GLIBC__
       gnu_iconv
      #endif
      ], [gl_func_iconv_gnu=yes], [gl_func_iconv_gnu=no])
    if test $gl_func_iconv_gnu = no; then
      iconv_flavor=
      case "$host_os" in
        aix*)  iconv_flavor=ICONV_FLAVOR_AIX ;;
        irix*) iconv_flavor=ICONV_FLAVOR_IRIX ;;
        hpux*) iconv_flavor=ICONV_FLAVOR_HPUX ;;
        osf*)  iconv_flavor=ICONV_FLAVOR_OSF ;;
      esac
      if test -n "$iconv_flavor"; then
        AC_DEFINE_UNQUOTED([ICONV_FLAVOR], [$iconv_flavor],
          [Define to a symbolic name denoting the flavor of iconv_open()
           implementation.])
        REPLACE_ICONV_OPEN=1
        AC_LIBOBJ([iconv_open])
        ICONV_H='iconv.h'
      fi
    fi
  fi
])

