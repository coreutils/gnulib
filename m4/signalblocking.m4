# signalblocking.m4 serial 2 (gettext-0.15.1)
dnl Copyright (C) 2001-2002, 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Determine available signal blocking primitives. Three different APIs exist:
# 1) POSIX: sigemptyset, sigaddset, sigprocmask
# 2) SYSV: sighold, sigrelse
# 3) BSD: sigblock, sigsetmask
# For simplicity, here we check only for the POSIX signal blocking.
AC_DEFUN([gl_SIGNALBLOCKING],
[
  signals_not_posix=
  AC_EGREP_HEADER(sigset_t, signal.h, , signals_not_posix=1)
  if test -z "$signals_not_posix"; then
    AC_CHECK_FUNC(sigprocmask, [gl_cv_func_sigprocmask=1])
  fi
  if test -n "$gl_cv_func_sigprocmask"; then
    AC_DEFINE([HAVE_POSIX_SIGNALBLOCKING], 1,
      [Define to 1 if you have the sigset_t type and the sigprocmask function.])
  else
    AC_LIBOBJ([sigprocmask])
    gl_PREREQ_SIGPROCMASK
  fi
])

# Prerequisites of lib/sigprocmask.c.
AC_DEFUN([gl_PREREQ_SIGPROCMASK], [
  AC_CHECK_FUNCS_ONCE(raise)
])
