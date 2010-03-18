# pty.m4 serial 1
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# gl_PTY_LIB
# ----------
# Define automake variable PTY_LIB to the library needed (if any).
AC_DEFUN([gl_PTY_LIB],
[
  # Check for the library required for forkpty.
  PTY_LIB=
  save_LIBS="$LIBS"
  AC_SEARCH_LIBS([forkpty], [util],
    [if test "$ac_cv_search_forkpty" != "none required"; then
       PTY_LIB="$ac_cv_search_forkpty"
     fi])
  LIBS="$save_LIBS"
  AC_SUBST([PTY_LIB])
])

AC_DEFUN([gl_FORKPTY],
[
  AC_REQUIRE([gl_PTY_LIB])
])

AC_DEFUN([gl_OPENPTY],
[
  AC_REQUIRE([gl_PTY_LIB])
])
