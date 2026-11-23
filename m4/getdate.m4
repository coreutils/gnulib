# getdate.m4
# serial 1
dnl Copyright (C) 2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_GETDATE],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl Persuade glibc and Solaris <time.h> to declare getdate.
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([gl_TIME_H_DEFAULTS])

  AC_CHECK_FUNCS_ONCE([getdate])
  if test $ac_cv_func_getdate = yes; then
    AC_CACHE_CHECK([whether getdate works],
      [gl_cv_func_getdate_works],
      [rm -f config.msk
       cat > config.msk <<\EOF
%m
%A %B %d, %Y, %H:%M:%S
%A
%B
%m/%d/%y %I %p
%d,%m,%Y %H:%M
%m/%d/%y
%d.%m.%y
%y-%m-%d
%A %H:%M:%S
%a
%b %a
%b %a %Y
%a %H
%b %H:%S
%H:%M
EOF
       DATEMSK=config.msk
       export DATEMSK
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stddef.h>
              #include <time.h>
            ]],
            [[int result = 0;
              struct tm *t;
              /* This test fails on macOS, NetBSD, Solaris 11 OmniOS.  */
              t = getdate ("10/1/87 4 PM");
              if (!(t != NULL
                    && t->tm_year == 1987 - 1900
                    && t->tm_mon == 10 - 1
                    && t->tm_mday == 1
                    && t->tm_hour == 16 && t->tm_min == 0 && t->tm_sec == 0))
                result |= 1;
              /* This test fails on musl libc, NetBSD, Solaris 11 OmniOS.  */
              t = getdate ("24,9,1986 10:30");
              if (!(t != NULL
                    && t->tm_year == 1986 - 1900
                    && t->tm_mon == 9 - 1
                    && t->tm_mday == 24
                    && t->tm_hour == 10 && t->tm_min == 30 && t->tm_sec == 0))
                result |= 2;
              /* This test fails on musl libc, AIX, Solaris 11 OmniOS.  */
              t = getdate ("13:30");
              if (!(t != NULL
                    && t->tm_mon >= 0 && t->tm_mon < 12
                    && t->tm_mday >= 1 && t->tm_mday <= 31
                    && t->tm_hour == 13 && t->tm_min == 30 && t->tm_sec == 0))
                result |= 4;
              getdate_err = -1;
              return result;
            ]])],
         [gl_cv_func_getdate_works=yes],
         [gl_cv_func_getdate_works=no],
         [case "$host_os" in
                # Guess yes on glibc systems.
            *-gnu* | gnu*)
                gl_cv_func_getdate_works="guessing yes" ;;
                # Guess no on musl.
            linux* | midipix*)
                gl_cv_func_getdate_works="guessing no" ;;
                # Guess no on macOS, NetBSD, AIX, Solaris 11 OmniOS.
            darwin* | nstbsd* | aix* | solaris*)
                gl_cv_func_getdate_works="guessing no" ;;
                # If we don't know, obey --enable-cross-guesses.
            *)  gl_cv_func_getdate_works="$gl_cross_guess_normal" ;;
          esac
         ])
       unset DATEMSK
       rm -f config.msk
      ])
    case "$gl_cv_func_getdate_works" in
      *no) REPLACE_GETDATE=1 ;;
    esac
  else
    HAVE_GETDATE=0
  fi
])

# Prerequisites of lib/getdate.c.
AC_DEFUN([gl_PREREQ_GETDATE],
[
  AC_REQUIRE([gl_TM_GMTOFF])
  :
])
