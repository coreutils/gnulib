# realloc.m4 serial 21
dnl Copyright (C) 2007, 2009-2021 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# This is adapted with modifications from upstream Autoconf here:
# https://git.savannah.gnu.org/cgit/autoconf.git/commit/?id=04be2b7a29d65d9a08e64e8e56e594c91749598c
AC_DEFUN([_AC_FUNC_REALLOC_IF],
[ AC_REQUIRE([AC_CANONICAL_HOST])dnl for cross-compiles
  AC_CACHE_CHECK([whether realloc (0, 0) returns nonnull],
    [ac_cv_func_realloc_0_nonnull],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <stdlib.h>
          ]],
          [[char *p = realloc (0, 0);
            int result = !p;
            free (p);
            return result;]])
       ],
       [ac_cv_func_realloc_0_nonnull=yes],
       [ac_cv_func_realloc_0_nonnull=no],
       [case "$host_os" in
          # Guess yes on platforms where we know the result.
          *-gnu* | gnu* | *-musl* | freebsd* | midnightbsd* | netbsd* | openbsd* \
          | hpux* | solaris* | cygwin* | mingw*)
            ac_cv_func_realloc_0_nonnull="guessing yes" ;;
          # If we don't know, obey --enable-cross-guesses.
          *) ac_cv_func_realloc_0_nonnull="$gl_cross_guess_normal" ;;
        esac
       ])
    ])
  case "$ac_cv_func_realloc_0_nonnull" in
    *yes)
      $1
      ;;
    *)
      $2
      ;;
  esac
])# AC_FUNC_REALLOC

# gl_FUNC_REALLOC_GNU
# -------------------
# Replace realloc if it is not compatible with GNU libc.
AC_DEFUN([gl_FUNC_REALLOC_GNU],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_REALLOC_POSIX])
  test $REPLACE_REALLOC = 1 || _AC_FUNC_REALLOC_IF([], [REPLACE_REALLOC=1])
])# gl_FUNC_REALLOC_GNU

# gl_FUNC_REALLOC_POSIX
# ---------------------
# Test whether 'realloc' is POSIX compliant (sets errno to ENOMEM when it
# fails, and doesn't mess up with ptrdiff_t overflow),
# and replace realloc if it is not.
AC_DEFUN([gl_FUNC_REALLOC_POSIX],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_MALLOC_POSIX])
  REPLACE_REALLOC=$REPLACE_MALLOC
])
