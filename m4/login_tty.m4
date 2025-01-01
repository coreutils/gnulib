# login_tty.m4
# serial 3
dnl Copyright (C) 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_LOGIN_TTY],
[
  AC_REQUIRE([gl_PTY_LIB])

  AC_CHECK_HEADERS_ONCE([utmp.h util.h libutil.h termios.h])
  gl_saved_libs="$LIBS"
  LIBS="$LIBS $PTY_LIB"
  gl_CHECK_FUNCS_ANDROID([login_tty], [[
    #include <sys/types.h>
    #if HAVE_UTMP_H
    # include <utmp.h>
    #endif
    #if HAVE_UTIL_H
    # include <util.h>
    #elif HAVE_LIBUTIL_H
    # include <libutil.h>
    #elif HAVE_TERMIOS_H
    # include <termios.h>
    #endif
  ]])
  LIBS="$gl_saved_LIBS"

  if test $ac_cv_func_login_tty = no; then
    HAVE_LOGIN_TTY=0
    case "$gl_cv_onwards_func_login_tty" in
      future*) REPLACE_LOGIN_TTY=1 ;;
    esac
  fi
])
