# time.m4 serial 1
dnl Copyright (C) 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

AC_DEFUN([gl_FUNC_TIME],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl glibc has the bug https://sourceware.org/bugzilla/show_bug.cgi?id=30200 .
  AC_CACHE_CHECK([whether time() works],
    [gl_cv_func_time_works],
    [dnl Guess that it works except on glibc >= 2.31 with Linux.
     dnl And binaries produced on glibc < 2.31 need to run fine on newer
     dnl glibc versions as well; therefore ignore __GLIBC_MINOR__.
     case "$host_os" in
       linux*-gnu*)
         AC_EGREP_CPP([Unlucky], [
           #include <features.h>
           #ifdef __GNU_LIBRARY__
            #if __GLIBC__ == 2
             Unlucky GNU user
            #endif
           #endif
           ],
           [gl_cv_func_time_works="guessing no"],
           [gl_cv_func_time_works="guessing yes"])
         ;;
       *) gl_cv_func_time_works="guessing yes";;
     esac
    ])
  case "$gl_cv_func_time_works" in
    *no) REPLACE_TIME=1 ;;
  esac
])

# Prerequisites of lib/time.c.
AC_DEFUN([gl_PREREQ_TIME],
[
  :
])
