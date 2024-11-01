# realloc.m4
# serial 36
dnl Copyright (C) 2007, 2009-2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# An an experimental option, the user can request a sanitized realloc()
# implementation, i.e. one that aborts upon undefined behaviour,
# by setting
#   gl_cv_func_realloc_sanitize=yes
# at configure time.
AC_DEFUN([gl_FUNC_REALLOC_SANITIZED],
[
  AC_CACHE_CHECK([whether realloc should abort upon undefined behaviour],
    [gl_cv_func_realloc_sanitize],
    [test -n "$gl_cv_func_realloc_sanitize" || gl_cv_func_realloc_sanitize=no])
])

m4_version_prereq([2.73], [], [
# This is copied from upstream Autoconf here:
# https://git.savannah.gnu.org/cgit/autoconf.git/tree/lib/autoconf/functions.m4?id=f8c82d292699fbce6d60abb46259a3781578f7fc#n1483
# _AC_FUNC_REALLOC_IF(IF-WORKS, IF-NOT[, UNKNOWN-ASSUME])
# -------------------------------------------------------
# If 'realloc (0, 0)' returns nonnull, run IF-WORKS, otherwise, IF-NOT.
# If it is not known whether it works, assume the shell word UNKNOWN-ASSUME,
# which should end in "yes" or in something else (the latter is the default).
AC_DEFUN([_AC_FUNC_REALLOC_IF],
[
  AC_REQUIRE([AC_CANONICAL_HOST])dnl for cross-compiles
  AC_CACHE_CHECK([whether realloc (0, 0) returns nonnull],
    [ac_cv_func_realloc_0_nonnull],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <stdlib.h>
            /* Use prealloc to test; 'volatile' prevents the compiler
               from optimizing the realloc call away.  */
            void *(*volatile prealloc) (void *, size_t) = realloc;]],
          [[void *p = prealloc (0, 0);
            int result = !p;
            free (p);
            return result;]])],
       [ac_cv_func_realloc_0_nonnull=yes],
       [ac_cv_func_realloc_0_nonnull=no],
       [AS_CASE([$host_os],
          [# Guess yes on platforms where we know the result.
           *-gnu* | freebsd* | netbsd* | openbsd* | bitrig* \
           | gnu* | *-musl* | midipix* | midnightbsd* \
           | hpux* | solaris* | cygwin* | mingw* | windows* | msys*],
            [ac_cv_func_realloc_0_nonnull="guessing yes"],
          [# Guess as follows if we don't know.
           ac_cv_func_realloc_0_nonnull=m4_default([$3], ["guessing no"])])])])
  AS_CASE([$ac_cv_func_realloc_0_nonnull], [*yes], [$1], [$2])
])# _AC_FUNC_REALLOC_IF
])

# gl_FUNC_REALLOC_POSIX
# ---------------------
# Test whether 'realloc' is POSIX compliant (sets errno to ENOMEM when it
# fails, and doesn't mess up with ptrdiff_t overflow),
# and replace realloc if it is not.
AC_DEFUN([gl_FUNC_REALLOC_POSIX],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_MALLOC_POSIX])
  AC_REQUIRE([gl_FUNC_REALLOC_SANITIZED])
  if test "$gl_cv_func_realloc_sanitize" != no; then
    REPLACE_REALLOC_FOR_REALLOC_POSIX=1
    AC_DEFINE([NEED_SANITIZED_REALLOC], [1],
      [Define to 1 if realloc should abort upon undefined behaviour.])
  else
    if test $REPLACE_MALLOC_FOR_MALLOC_POSIX = 1; then
      REPLACE_REALLOC_FOR_REALLOC_POSIX=1
    fi
  fi
])
