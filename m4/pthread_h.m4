# pthread_h.m4 serial 2
dnl Copyright (C) 2009-2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_PTHREAD_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_PTHREAD_H_DEFAULTS])

  AC_REQUIRE([gl_THREADLIB])

  gl_CHECK_NEXT_HEADERS([pthread.h])
  dnl On mingw, if --enable-threads=windows or gl_AVOID_WINPTHREAD is used,
  dnl ignore the <pthread.h> from the mingw-w64 winpthreads library.
  if test $ac_cv_header_pthread_h = yes && test $gl_threads_api != windows; then
    HAVE_PTHREAD_H=1
  else
    HAVE_PTHREAD_H=0
  fi
  AC_SUBST([HAVE_PTHREAD_H])

  AC_CHECK_TYPES([pthread_t, pthread_spinlock_t], [], [],
    [AC_INCLUDES_DEFAULT[
     #if HAVE_PTHREAD_H
      #include <pthread.h>
     #endif]])
  if test $ac_cv_type_pthread_t != yes; then
    HAVE_PTHREAD_T=0
  fi
  if test $ac_cv_type_pthread_spinlock_t != yes; then
    HAVE_PTHREAD_SPINLOCK_T=0
  fi

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, if it is not common
  dnl enough to be declared everywhere.
  gl_WARN_ON_USE_PREPARE([[#include <pthread.h>
    ]], [pthread_mutex_timedlock])

  AC_REQUIRE([AC_C_RESTRICT])

  dnl For backward compatibility with gnulib versions <= 2019-07.
  LIB_PTHREAD="$LIBMULTITHREAD"
  AC_SUBST([LIB_PTHREAD])
])

AC_DEFUN([gl_PTHREAD_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_PTHREAD_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_PTHREAD_H_DEFAULTS],
[
  GNULIB_PTHREAD=0;                 AC_SUBST([GNULIB_PTHREAD])
  GNULIB_PTHREAD_MUTEX_TIMEDLOCK=0; AC_SUBST([GNULIB_PTHREAD_MUTEX_TIMEDLOCK])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_PTHREAD_MUTEX_TIMEDLOCK=1;   AC_SUBST([HAVE_PTHREAD_MUTEX_TIMEDLOCK])
  HAVE_PTHREAD_SPINLOCK_T=1;        AC_SUBST([HAVE_PTHREAD_SPINLOCK_T])
  HAVE_PTHREAD_T=1;                 AC_SUBST([HAVE_PTHREAD_T])
])
