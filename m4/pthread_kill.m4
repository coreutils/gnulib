# pthread_kill.m4
# serial 1
dnl Copyright (C) 2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Set PTHREAD_KILL_LIB.
AC_DEFUN([gl_FUNC_PTHREAD_KILL],
[
  AC_REQUIRE([gl_PTHREADLIB])

  AC_CHECK_FUNCS_ONCE([pthread_kill])

  PTHREAD_KILL_LIB=
  if test $ac_cv_func_pthread_kill = yes; then
    dnl pthread_kill is available without -lpthread.
    :
  else
    if test -n "$LIBPMULTITHREAD"; then
      AC_CACHE_CHECK([for pthread_kill in $LIBPMULTITHREAD],
        [gl_cv_func_pthread_kill_in_LIBPMULTITHREAD],
        [gl_saved_LIBS="$LIBS"
         LIBS="$LIBS $LIBPMULTITHREAD"
         AC_LINK_IFELSE(
           [AC_LANG_PROGRAM(
              [[#include <pthread.h>
                #include <signal.h>
              ]],
              [[return pthread_kill (pthread_self (), SIGINT);]])
           ],
           [gl_cv_func_pthread_kill_in_LIBPMULTITHREAD=yes],
           [gl_cv_func_pthread_kill_in_LIBPMULTITHREAD=no])
         LIBS="$gl_saved_LIBS"
        ])
      if test $gl_cv_func_pthread_kill_in_LIBPMULTITHREAD = yes; then
        dnl pthread_kill is available with -pthread or -lpthread.
        PTHREAD_KILL_LIB="$LIBPMULTITHREAD"
      else
        dnl pthread_kill is not available at all.
        :
      fi
    else
      dnl pthread_kill is not available at all.
      :
    fi
  fi
  AC_SUBST([PTHREAD_KILL_LIB])
])
