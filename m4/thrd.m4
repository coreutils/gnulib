# thrd.m4
# serial 7
dnl Copyright (C) 2019-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_THRD_CREATE],
[
  AC_REQUIRE([gl_THREADS_H])
  gl_CHECK_FUNCS_ANDROID([thrd_create], [[#include <threads.h>]])
  case "$gl_cv_onwards_func_thrd_create" in
    future*)
      REPLACE_THRD_CREATE=1
      dnl Assume that the other thrd_* functions are introduced at the same time
      dnl as thrd_create.
      REPLACE_THRD_CURRENT=1
      REPLACE_THRD_DETACH=1
      REPLACE_THRD_EQUAL=1
      REPLACE_THRD_EXIT=1
      REPLACE_THRD_JOIN=1
      REPLACE_THRD_SLEEP=1
      REPLACE_THRD_YIELD=1
      ;;
  esac
])

AC_DEFUN([gl_FUNC_THRD_JOIN],
[
  AC_REQUIRE([gl_THREADS_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  if test $ac_cv_header_threads_h = yes; then
    if test $BROKEN_THRD_START_T = 1 || test $BROKEN_THRD_JOIN = 1; then
      REPLACE_THRD_CREATE=1
      REPLACE_THRD_CURRENT=1
      REPLACE_THRD_DETACH=1
      REPLACE_THRD_EQUAL=1
      REPLACE_THRD_EXIT=1
      REPLACE_THRD_JOIN=1
      if test $BROKEN_THRD_START_T = 1; then
        AC_DEFINE([BROKEN_THRD_START_T], [1],
          [Define if the thrd_start_t type is not as described in ISO C 11.])
      else
        AC_DEFINE([BROKEN_THRD_JOIN], [1],
          [Define if thrd_start_t is OK but thrd_join discards the thread's exit code.])
      fi
      dnl The thrd_exit replacement relies on pthread_exit, which on AIX is in
      dnl libpthread.
      LIBSTDTHREAD="$LIBSTDTHREAD $LIBPTHREAD"
    fi

    dnl On Solaris 11.4, thrd_join crashes when the second argument is NULL.
    AC_CACHE_CHECK([whether thrd_join with NULL argument works],
      [gl_cv_func_thrd_join_null_works],
      [saved_LIBS="$LIBS"
       LIBS="$LIBS $LIBSTDTHREAD"
       AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stddef.h>
              #include <threads.h>
              static int func (void *arg)
              {
                return (arg != NULL);
              }
            ]],
            [[thrd_t thread;
              if (thrd_create (&thread, func, NULL) != thrd_success)
                return 1;
              if (thrd_join (thread, NULL) != thrd_success)
                return 2;
              return 0;
            ]])],
         [gl_cv_func_thrd_join_null_works=yes],
         [gl_cv_func_thrd_join_null_works=no],
         [case "$host_os" in
                      # Only Solaris is known to be broken.
            solaris*) gl_cv_func_thrd_join_null_works="guessing no" ;;
            *)        gl_cv_func_thrd_join_null_works="guessing yes" ;;
          esac
         ])
       LIBS="$saved_LIBS"
      ])
    case "$gl_cv_func_thrd_join_null_works" in
      *yes) ;;
      *)
        REPLACE_THRD_JOIN=1
        AC_DEFINE([BROKEN_THRD_JOIN_NULL], [1],
          [Define if the thrd_join function, when given a NULL argument, does not behave as in ISO C 11.])
        ;;
    esac
  fi
])
