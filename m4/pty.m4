# pty.m4 serial 7
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

AC_DEFUN([gl_FUNC_FORKPTY],
[
  AC_REQUIRE([gl_PTY_LIB])
  AC_REQUIRE([gl_PTY_H])

  dnl We assume that forkpty exists (possibly in libc, possibly in libutil)
  dnl if and only if it is declared.
  AC_CHECK_DECLS([forkpty],,, [[
#if HAVE_PTY_H
# include <pty.h>
#endif
#if HAVE_UTIL_H
# include <util.h>
#endif
#if HAVE_LIBUTIL_H
# include <libutil.h>
#endif
]])
  if test $ac_cv_have_decl_forkpty = yes; then
    dnl The system has forkpty.
    dnl Prefer glibc's const-safe prototype, if available.
    AC_CACHE_CHECK([for const-safe forkpty signature],
      [gl_cv_func_forkpty_const],
      [AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[
#if HAVE_PTY_H
# include <pty.h>
#endif
#if HAVE_UTIL_H
# include <util.h>
#endif
#if HAVE_LIBUTIL_H
# include <libutil.h>
#endif
          ]], [[
            int forkpty (int *, char *, struct termios const *,
                         struct winsize const *);
          ]])
        ],
        [gl_cv_func_forkpty_const=yes], [gl_cv_func_forkpty_const=no])
      ])
    if test $gl_cv_func_forkpty_const != yes; then
      REPLACE_FORKPTY=1
      AC_LIBOBJ([forkpty])
    fi
  else
    dnl The system does not have forkpty.
    HAVE_FORKPTY=0
    AC_LIBOBJ([forkpty])
  fi
])

AC_DEFUN([gl_FUNC_OPENPTY],
[
  AC_REQUIRE([gl_PTY_LIB])
  AC_REQUIRE([gl_PTY_H])

  dnl Persuade Solaris <stdlib.h> to declare posix_openpt().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  dnl We assume that openpty exists (possibly in libc, possibly in libutil)
  dnl if and only if it is declared.
  AC_CHECK_DECLS([openpty],,, [[
#if HAVE_PTY_H
# include <pty.h>
#endif
#if HAVE_UTIL_H
# include <util.h>
#endif
#if HAVE_LIBUTIL_H
# include <libutil.h>
#endif
]])
  if test $ac_cv_have_decl_openpty = yes; then
    dnl The system has openpty.
    dnl Prefer glibc's const-safe prototype, if available.
    AC_CACHE_CHECK([for const-safe openpty signature],
      [gl_cv_func_openpty_const],
      [AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[
#if HAVE_PTY_H
# include <pty.h>
#endif
#if HAVE_UTIL_H
# include <util.h>
#endif
#if HAVE_LIBUTIL_H
# include <libutil.h>
#endif
          ]], [[
            int openpty (int *, int *, char *, struct termios const *,
                       struct winsize const *);
          ]])
        ],
        [gl_cv_func_openpty_const=yes], [gl_cv_func_openpty_const=no])
      ])
    if test $gl_cv_func_openpty_const != yes; then
      REPLACE_OPENPTY=1
      AC_LIBOBJ([openpty])
      AC_DEFINE([HAVE_OPENPTY], [1],
        [Define to 1 if the system has the 'openpty' function.])
    fi
  else
    dnl The system does not have openpty.
    HAVE_OPENPTY=0
    AC_LIBOBJ([openpty])
    AC_CHECK_FUNCS([_getpty posix_openpt])
  fi
])

AC_DEFUN([gl_FUNC_LOGIN_TTY],
[
  AC_REQUIRE([gl_PTY_LIB])

  AC_CHECK_FUNCS_ONCE([login_tty])
  if test $ac_cv_func_login_tty = no; then
    AC_LIBOBJ([login_tty])
  fi
])
