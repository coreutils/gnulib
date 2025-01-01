# termios_h.m4
# serial 8
dnl Copyright (C) 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_TERMIOS_H],
[
  dnl Ensure to expand the default settings once only, before all statements
  dnl that occur in other macros.
  AC_REQUIRE([gl_TERMIOS_H_DEFAULTS])

  gl_CHECK_NEXT_HEADERS([termios.h])
  if test $ac_cv_header_termios_h != yes; then
    HAVE_TERMIOS_H=0
  fi

  if test $ac_cv_header_termios_h = yes; then
    AC_CACHE_CHECK([for struct winsize in <termios.h>],
      [gl_cv_struct_winsize_in_termios_h],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <termios.h>
            ]],
            [[static struct winsize x; x.ws_row = x.ws_col;]])],
         [gl_cv_struct_winsize_in_termios_h=yes],
         [gl_cv_struct_winsize_in_termios_h=no])
    ])
    if test $gl_cv_struct_winsize_in_termios_h = no; then
      AC_CHECK_HEADERS_ONCE([sys/ioctl.h])
      if test $ac_cv_header_sys_ioctl_h = yes; then
        AC_CACHE_CHECK([for struct winsize in <sys/ioctl.h>],
          [gl_cv_struct_winsize_in_sys_ioctl_h],
          [AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM(
                [[#include <sys/ioctl.h>
                ]],
                [[static struct winsize x; x.ws_row = x.ws_col;]])],
             [gl_cv_struct_winsize_in_sys_ioctl_h=yes],
             [gl_cv_struct_winsize_in_sys_ioctl_h=no])
        ])
      fi
    fi
  fi

  TERMIOS_H_DEFINES_STRUCT_WINSIZE=0
  SYS_IOCTL_H_DEFINES_STRUCT_WINSIZE=0
  if test "$gl_cv_struct_winsize_in_termios_h" != yes \
     || test "$gl_cv_struct_winsize_in_sys_ioctl_h" != yes; then
    if test "$gl_cv_struct_winsize_in_termios_h" = yes; then
      TERMIOS_H_DEFINES_STRUCT_WINSIZE=1
    elif test "$gl_cv_struct_winsize_in_sys_ioctl_h" = yes; then
      SYS_IOCTL_H_DEFINES_STRUCT_WINSIZE=1
    fi
  fi

  dnl Ensure the type pid_t gets defined.
  AC_REQUIRE([AC_TYPE_PID_T])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, and which is not
  dnl guaranteed by C89.
  gl_WARN_ON_USE_PREPARE([[#include <termios.h>]],
    [tcgetsid])

  AC_SUBST([TERMIOS_H_DEFINES_STRUCT_WINSIZE])
  AC_SUBST([SYS_IOCTL_H_DEFINES_STRUCT_WINSIZE])
])

# gl_TERMIOS_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_TERMIOS_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_TERMIOS_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_TERMIOS_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_TERMIOS_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_TCGETSID])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_TERMIOS_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_TERMIOS_H_DEFAULTS])
])

AC_DEFUN([gl_TERMIOS_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_DECL_TCGETSID=1;   AC_SUBST([HAVE_DECL_TCGETSID])
  HAVE_TERMIOS_H=1;       AC_SUBST([HAVE_TERMIOS_H])
])
