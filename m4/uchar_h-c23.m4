# uchar_h-c23.m4
# serial 1
dnl Copyright (C) 2023-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_UCHAR_H_C23],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([AM_ICONV])
  AC_DEFINE([GL_CHAR32_T_IS_UNICODE], [1],
    [Define if gnulib's char32_t values are always Unicode code points.])
  dnl On macOS, FreeBSD, NetBSD, Solaris, the functions mbrtoc32 and c32rtomb
  dnl need to convert between the wchar_t encoding and Unicode.
  gl_char32_t_vs_wchar_t_needs_conversion=no
  case "$host_os" in
    darwin* | freebsd* | dragonfly* | netbsd* | solaris*)
      gl_char32_t_vs_wchar_t_needs_conversion=yes
      AC_DEFINE([GL_CHAR32_T_VS_WCHAR_T_NEEDS_CONVERSION], [1],
        [Define if gnulib needs to convert between the wchar_t encoding and Unicode.])
      LIBC32CONV="$LIBICONV"
      LTLIBC32CONV="$LTLIBICONV"
      ;;
    *)
      LIBC32CONV=
      LTLIBC32CONV=
      ;;
  esac
  AC_SUBST([LIBC32CONV])
  AC_SUBST([LTLIBC32CONV])
])
