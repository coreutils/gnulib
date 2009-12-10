# pty.m4 serial 1
dnl Copyright (C) 2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# gl_PTY
# ------
# Make sure that pty.h provides forkpty, or sets up a replacement header.
# Also define automake variable PTY_LIB to the library needed (if any).
AC_DEFUN([gl_PTY],
[
  PTY_H=''
  PTY_LIB=''
  # First make sure that pty.h provides forkpty, or setup the replacement.
  AC_CHECK_HEADERS_ONCE([pty.h])
  if test $ac_cv_header_pty_h != yes; then
    AC_CHECK_DECL([forkpty],,, [[#include <util.h>]])
    if test $ac_cv_have_decl_forkpty = no; then
      AC_CHECK_DECL([forkpty],,, [[#include <libutil.h>]])
      if test $ac_cv_have_decl_forkpty = no; then
        AC_MSG_WARN([[Cannot find forkpty, build will likely fail]])
      else
        PTY_H='pty.h'
	PTY_HEADER='libutil.h'
      fi
    else
      PTY_H='pty.h'
      PTY_HEADER='util.h'
    fi
  fi
  # Second check for the library required for forkpty.
  save_LIBS="$LIBS"
  AC_SEARCH_LIBS([forkpty], [util],
    [if test "$ac_cv_search_forkpty" != "none required"; then
       PTY_LIB="$ac_cv_search_forkpty"
     fi])
  LIBS="$save_LIBS"
  AC_SUBST([PTY_H])
  AC_SUBST([PTY_LIB])
  AC_SUBST([PTY_HEADER])
])
