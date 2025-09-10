# ptsname_r.m4
# serial 13
dnl Copyright (C) 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_PTSNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare ptsname_r().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([ptsname_r])
  if test $ac_cv_func_ptsname_r = no; then
    HAVE_PTSNAME_R=0
  else
    AC_DEFINE([HAVE_ESSENTIALLY_WORKING_PTSNAME_R], [1],
      [Define to 1 if ptsname_r() is essentially working.])
    dnl On FreeBSD 13.0 and Android 4.3, when ptsname_r fails, it returns -1
    dnl instead of the error code.
    AC_REQUIRE([AC_CANONICAL_HOST])
    AC_CACHE_CHECK([whether ptsname_r returns an error code],
      [gl_cv_func_ptsname_r_retval_ok],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
#include <stdlib.h>
int
main (void)
{
  char buf[80];
  return ptsname_r (-1, buf, sizeof buf) == -1;
}]])],
         [gl_cv_func_ptsname_r_retval_ok=yes],
         [gl_cv_func_ptsname_r_retval_ok=no],
         [case "$host_os" in
            dnl Guess no on FreeBSD, Android.
            freebsd* | dragonfly* | midnightbsd* | linux*-android*)
              gl_cv_func_ptsname_r_retval_ok="guessing no" ;;
            dnl Guess yes otherwise.
            *)
              gl_cv_func_ptsname_r_retval_ok="guessing yes" ;;
          esac
         ])
      ])
    case "$gl_cv_func_ptsname_r_retval_ok" in
      *yes) ;;
      *) REPLACE_PTSNAME_R=1 ;;
    esac
    dnl On NetBSD 10.0, when ptsname_r fails with ERANGE, it clobbers the
    dnl result buffer. We don't use an AC_RUN_IFELSE test here, because
    dnl while the bug exists on all platforms, only NetBSD/i386 has the
    dnl files /dev/ptyp[01] on which the bug becomes apparent.
    dnl
    dnl On Solaris 11 OmniOS the result buffer is clobbered as well.
    case "$host_os" in
      netbsd* | solaris*) REPLACE_PTSNAME_R=1 ;;
    esac
  fi

  AC_REQUIRE([AC_HEADER_MAJOR])
])

# Prerequisites of lib/ptsname.c.
AC_DEFUN([gl_PREREQ_PTSNAME_R], [
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether isatty sets errno when it fails],
    [gl_cv_func_isatty_sets_errno],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <errno.h>
            #if defined _WIN32 && ! defined __CYGWIN__
            # include <io.h>
            #else
            # include <unistd.h>
            #endif
          ]GL_MDA_DEFINES],
          [[errno = 0;
            isatty (-1);
            return errno == 0;
          ]])
       ],
       [gl_cv_func_isatty_sets_errno=yes],
       [gl_cv_func_isatty_sets_errno=no],
       [case "$host_os" in
          solaris*)
            gl_cv_func_isatty_sets_errno="guessing no" ;;
            # Guess yes on mingw, no on MSVC.
          mingw* | windows*)
            AC_EGREP_CPP([Known], [
#ifdef __MINGW32__
 Known
#endif
              ],
              [gl_cv_func_isatty_sets_errno="guessing yes"],
              [gl_cv_func_isatty_sets_errno="guessing no"])
            ;;
          *)
            gl_cv_func_isatty_sets_errno="guessing yes" ;;
        esac
       ])
    ])
  case "$gl_cv_func_isatty_sets_errno" in
    *yes) ;;
    *)
      AC_DEFINE([ISATTY_FAILS_WITHOUT_SETTING_ERRNO], [1],
        [Define to 1 if isatty() may fail without setting errno.])
      ;;
  esac
])
