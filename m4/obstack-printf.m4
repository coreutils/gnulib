# obstack-printf.m4
# serial 5
dnl Copyright (C) 2008-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl From Eric Blake.
dnl Test whether obstack_printf() exists.  For now, we assume that
dnl obstack_vprintf behaves identically, so we only test for one.

AC_DEFUN([gl_FUNC_OBSTACK_PRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_OBSTACK])

  dnl Persuade glibc <stdio.h> to declare obstack_printf(), obstack_vprintf().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([obstack_printf])
  dnl The obstack_printf function from glibc does not work with the
  dnl 'struct obstack' defined in gnulib's <obstack.h>.  Therefore, when
  dnl overriding 'struct obstack', we must also override obstack_printf.
  if test $HAVE_OBSTACK = 0 || test $REPLACE_OBSTACK = 1; then
    gl_REPLACE_OBSTACK_PRINTF
  fi

  gl_DECL_OBSTACK_PRINTF
])

AC_DEFUN([gl_REPLACE_OBSTACK_PRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  if test $ac_cv_func_obstack_printf = yes; then
    REPLACE_OBSTACK_PRINTF=1
  fi
])

dnl Ensure obstack_printf() and obstack_vprintf() are declared
dnl (whether or not they are being replaced).
AC_DEFUN([gl_DECL_OBSTACK_PRINTF],
[
  AC_CHECK_DECLS_ONCE([obstack_printf])
  if test $ac_cv_have_decl_obstack_printf = no; then
    HAVE_DECL_OBSTACK_PRINTF=0
  fi
])
