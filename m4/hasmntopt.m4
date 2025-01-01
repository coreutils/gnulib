# hasmntopt.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_HASMNTOPT],
[
  AC_REQUIRE([gl_MNTENT_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  gl_CHECK_FUNCS_ANDROID([setmntent],
    [[#include <stdio.h>
      #include <mntent.h>
    ]])
  if test $ac_cv_func_setmntent != yes; then
    HAVE_SETMNTENT=0
  fi

  gl_CHECK_FUNCS_ANDROID([hasmntopt],
    [[#include <stdio.h>
      #include <mntent.h>
    ]])
  if test $ac_cv_func_hasmntopt = yes; then
    dnl hasmntopt has the dumbest possible implementation on musl libc 1.2.5:
    dnl it confuses the "atime" option with the "noatime" option.
    AC_CACHE_CHECK([whether hasmntopt works],
      [gl_cv_func_hasmntopt_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <mntent.h>
              #include <stddef.h>
            ]],
            [[int result = 0;
              /* This test fails on musl libc 1.2.5.  */
              {
                char options[] = "rw,noatime";
                struct mntent me;
                me.mnt_opts = options;
                if (hasmntopt (&me, "atime") != NULL)
                  result |= 1;
              }
              return result;
            ]])
         ],
         [gl_cv_func_hasmntopt_works=yes],
         [gl_cv_func_hasmntopt_works=no],
         [case "$host_os" in
                                # Guess yes on glibc systems.
            *-gnu* | gnu*)      gl_cv_func_hasmntopt_works="guessing yes" ;;
                                # Guess no on musl systems.
            *-musl* | midipix*) gl_cv_func_hasmntopt_works="guessing no" ;;
            *)                  gl_cv_func_hasmntopt_works="$gl_cross_guess_normal" ;;
          esac
         ])
    ])
    case "$gl_cv_func_hasmntopt_works" in
      *yes) ;;
      *)    REPLACE_HASMNTOPT=1 ;;
    esac
  else
    HAVE_HASMNTOPT=0
    case "$gl_cv_onwards_func_hasmntopt" in
      future*) REPLACE_HASMNTOPT=1 ;;
    esac
  fi
])

# Prerequisites of lib/hasmntopt.c.
AC_DEFUN([gl_PREREQ_HASMNTOPT],
[
  :
])
