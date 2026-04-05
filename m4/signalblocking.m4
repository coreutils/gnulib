# signalblocking.m4
# serial 18
dnl Copyright (C) 2001-2002, 2006-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Determine available signal blocking primitives. Three different APIs exist:
# 1) POSIX: sigemptyset, sigaddset, sigprocmask
# 2) SYSV: sighold, sigrelse
# 3) BSD: sigblock, sigsetmask
# For simplicity, here we check only for the POSIX signal blocking.
AC_DEFUN([gl_SIGNALBLOCKING],
[
  AC_REQUIRE([gl_SIGNAL_H_DEFAULTS])
  AC_REQUIRE([gl_CHECK_TYPE_SIGSET_T])
  AC_CHECK_FUNCS_ONCE([sigprocmask])
  HAVE_POSIX_SIGNALBLOCKING=0
  AS_CASE([$gl_cv_type_sigset_t,$ac_cv_func_sigprocmask],
    [yes,yes], [HAVE_POSIX_SIGNALBLOCKING=1])
])

# Prerequisites of lib/sigprocmask.c.
AC_DEFUN([gl_PREREQ_SIGPROCMASK], [:])
