# strptime.m4
# serial 10
dnl Copyright (C) 2007, 2009-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_FUNC_STRPTIME_POSIX],
[
  AC_REQUIRE([gl_TIME_H_DEFAULTS])
  AC_REQUIRE([AC_C_RESTRICT])
  AC_CHECK_FUNCS_ONCE([strptime])
  if test $ac_cv_func_strptime != yes; then
    HAVE_STRPTIME=0
  fi
])

AC_DEFUN([gl_FUNC_STRPTIME_GNU],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl Persuade glibc and Solaris <time.h> to declare strptime.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_FUNC_STRPTIME_POSIX
  if test $ac_cv_func_strptime = yes; then
    AC_CACHE_CHECK([whether strptime works with incomplete format strings],
      [gl_cv_func_strptime_gnu],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <string.h>
              #include <time.h>
            ]],
            [[int result = 0;
              { /* This test fails on Solaris.  */
                struct tm t;
                memset (&t, '\0', sizeof (t));
                t.tm_year = -9999; t.tm_mon = -9999; t.tm_mday = -9999;
                t.tm_hour = -9999; t.tm_min = -9999; t.tm_sec = -9999;
                t.tm_isdst = -1;
                const char *end =
                  strptime ("Friday September 18, 1987", "%A %B %d, %Y", &t);
                if (!(end != NULL && *end == '\0'
                      && t.tm_hour == -9999 && t.tm_min == -9999 && t.tm_sec == -9999))
                  result |= 1;
              }
              { /* This test fails on Solaris.  */
                struct tm t;
                memset (&t, '\0', sizeof (t));
                t.tm_year = -9999; t.tm_mon = -9999; t.tm_mday = -9999;
                t.tm_hour = -9999; t.tm_min = -9999; t.tm_sec = -9999;
                t.tm_isdst = -1;
                const char *end = strptime ("10:29:30", "%H:%M:%S", &t);
                if (!(end != NULL && *end == '\0'
                      && t.tm_year == -9999 && t.tm_mon == -9999 && t.tm_mday == -9999))
                  result |= 2;
              }
              return result;
            ]])],
         [gl_cv_func_strptime_gnu=yes],
         [gl_cv_func_strptime_gnu=no],
         [case "$host_os" in
                      # Guess no on Solaris.
            solaris*) gl_cv_func_strptime_gnu="guessing no" ;;
                      # If we don't know, obey --enable-cross-guesses.
            *)        gl_cv_func_strptime_gnu="$gl_cross_guess_normal" ;;
          esac
         ])
      ])
    case "$gl_cv_func_strptime_gnu" in
      *no) REPLACE_STRPTIME=1 ;;
    esac
  fi
])

# Prerequisites of lib/strptime.c.
AC_DEFUN([gl_PREREQ_STRPTIME],
[
  AC_REQUIRE([gl_TM_GMTOFF])
  :
])
