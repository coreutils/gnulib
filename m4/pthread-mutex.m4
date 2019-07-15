# pthread-mutex.m4 serial 1
dnl Copyright (C) 2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_PTHREAD_MUTEX],
[
  AC_REQUIRE([gl_PTHREAD_H])
  AC_REQUIRE([AC_CANONICAL_HOST])

  if { case "$host_os" in mingw*) true;; *) false;; esac; } \
     && test $gl_threads_api = windows; then
    dnl Choose function names that don't conflict with the mingw-w64 winpthreads
    dnl library.
    REPLACE_PTHREAD_MUTEX_INIT=1
    REPLACE_PTHREAD_MUTEXATTR_INIT=1
    REPLACE_PTHREAD_MUTEXATTR_GETTYPE=1
    REPLACE_PTHREAD_MUTEXATTR_SETTYPE=1
    REPLACE_PTHREAD_MUTEXATTR_GETROBUST=1
    REPLACE_PTHREAD_MUTEXATTR_SETROBUST=1
    REPLACE_PTHREAD_MUTEXATTR_DESTROY=1
    REPLACE_PTHREAD_MUTEX_LOCK=1
    REPLACE_PTHREAD_MUTEX_TRYLOCK=1
    REPLACE_PTHREAD_MUTEX_TIMEDLOCK=1
    REPLACE_PTHREAD_MUTEX_UNLOCK=1
    REPLACE_PTHREAD_MUTEX_DESTROY=1
  else
    if test $HAVE_PTHREAD_H = 0; then
      HAVE_PTHREAD_MUTEX_INIT=0
      HAVE_PTHREAD_MUTEXATTR_INIT=0
      HAVE_PTHREAD_MUTEXATTR_GETTYPE=0
      HAVE_PTHREAD_MUTEXATTR_SETTYPE=0
      HAVE_PTHREAD_MUTEXATTR_GETROBUST=0
      HAVE_PTHREAD_MUTEXATTR_SETROBUST=0
      HAVE_PTHREAD_MUTEXATTR_DESTROY=0
      HAVE_PTHREAD_MUTEX_LOCK=0
      HAVE_PTHREAD_MUTEX_TRYLOCK=0
      dnl HAVE_PTHREAD_MUTEX_TIMEDLOCK is set in pthread_mutex_timedlock.m4.
      HAVE_PTHREAD_MUTEX_UNLOCK=0
      HAVE_PTHREAD_MUTEX_DESTROY=0
    fi
  fi
])
