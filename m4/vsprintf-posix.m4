# vsprintf-posix.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_VSPRINTF_POSIX],
[
  AC_REQUIRE([gl_EOVERFLOW])
  AC_REQUIRE([gl_PRINTF_SIZES_C99])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_A])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_N])
  AC_REQUIRE([gl_PRINTF_POSITIONS])
  if expr "$gl_cv_func_printf_sizes_c99" : ".*yes" > /dev/null \
     && expr "$gl_cv_func_printf_directive_a" : ".*yes" > /dev/null \
     && expr "$gl_cv_func_printf_directive_n" : ".*yes" > /dev/null \
     && expr "$gl_cv_func_printf_positions" : ".*yes" > /dev/null; then
    : # vsprintf exists and is already POSIX compliant.
  else
    if ! expr "$gl_cv_func_printf_directive_a" : ".*yes" > /dev/null; then
      AC_DEFINE([NEED_PRINTF_DIRECTIVE_A], 1,
        [Define if the vasnprintf implementation needs special code for
         the 'a' and 'A' directives.])
    fi
    gl_REPLACE_VASNPRINTF
    gl_REPLACE_VSPRINTF
  fi
])

AC_DEFUN([gl_REPLACE_VSPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_LIBOBJ([vsprintf])
  REPLACE_VSPRINTF=1
  gl_PREREQ_VSPRINTF
])

AC_DEFUN([gl_PREREQ_VSPRINTF], [:])
