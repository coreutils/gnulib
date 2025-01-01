# eealloc.m4
# serial 6
dnl Copyright (C) 2003, 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_EEALLOC],
[
  AC_REQUIRE([gl_EEMALLOC])
  AC_REQUIRE([gl_EEREALLOC])
])

AC_DEFUN([gl_EEMALLOC],
[
  _AC_FUNC_MALLOC_IF(
    [gl_cv_func_malloc_0_nonnull=1],
    [gl_cv_func_malloc_0_nonnull=0],
    ["$gl_cross_guess_normal"])
  AC_DEFINE_UNQUOTED([MALLOC_0_IS_NONNULL], [$gl_cv_func_malloc_0_nonnull],
    [If malloc(0) is != NULL, define this to 1.  Otherwise define this
     to 0.])
])

AC_DEFUN([gl_EEREALLOC],
[
  _AC_FUNC_REALLOC_IF(
    [gl_cv_func_realloc_0_nonnull=1],
    [gl_cv_func_realloc_0_nonnull=0],
    ["$gl_cross_guess_normal"])
  AC_DEFINE_UNQUOTED([REALLOC_0_IS_NONNULL], [$gl_cv_func_realloc_0_nonnull],
    [If realloc(NULL,0) is != NULL, define this to 1.  Otherwise define this
     to 0.])
])

m4_version_prereq([2.73], [], [
# This is copied from upstream Autoconf here:
# https://git.savannah.gnu.org/cgit/autoconf.git/tree/lib/autoconf/functions.m4?id=1f38316f6af7bf63e5e7dd187ff6456e07ad743e#n1483
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
