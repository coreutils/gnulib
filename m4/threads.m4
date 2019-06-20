# threads.m4 serial 1
dnl Copyright (C) 2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_THREADS_H],
[
  AC_REQUIRE([gl_THREADS_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([gl_THREADLIB_BODY])
  AC_REQUIRE([gl_YIELD])

  gl_CHECK_NEXT_HEADERS([threads.h])
  if test $ac_cv_header_threads_h = yes; then
    HAVE_THREADS_H=1
  else
    HAVE_THREADS_H=0
  fi
  AC_SUBST([HAVE_THREADS_H])

  if test $HAVE_THREADS_H = 1; then
    dnl AIX 7.1..7.2 defines thrd_start_t incorrectly, namely as
    dnl 'void * (*) (void *)' instead of 'int (*) (void *)'.
    AC_CACHE_CHECK([whether thrd_start_t is correct],
      [gl_cv_thrd_start_t_correct],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM([[
            #include <threads.h>
            #ifdef __cplusplus
            extern "C" {
            #endif
            extern void foo (thrd_start_t);
            extern void foo (int (*) (void *));
            #ifdef __cplusplus
            }
            #endif
          ]], [[]])],
         [gl_cv_thrd_start_t_correct=yes],
         [gl_cv_thrd_start_t_correct=no])
      ])
    if test $gl_cv_thrd_start_t_correct != yes; then
      BROKEN_THRD_START_T=1
    fi
  fi

  case "$host_os" in
    mingw*)
      LIBSTDTHREAD=
      LTLIBSTDTHREAD=
      ;;
    *)
      if test $ac_cv_header_threads_h = yes; then
        dnl glibc >= 2.29 has thrd_create in libpthread.
        dnl FreeBSD >= 10 has thrd_create in libstdthreads.
        dnl AIX >= 7.1 and Solaris >= 11.4 have thrd_create in libc.
        AC_CHECK_FUNCS([thrd_create])
        if test $ac_cv_func_thrd_create = yes; then
          LIBSTDTHREAD=
          LTLIBSTDTHREAD=
        else
          AC_CHECK_LIB([stdthreads], [thrd_create], [
            LIBSTDTHREAD='-lstdthreads'
            LTLIBSTDTHREAD='-lstdthreads'
          ], [
            dnl Guess that thrd_create is in libpthread.
            LIBSTDTHREAD="$LIBMULTITHREAD"
            LTLIBSTDTHREAD="$LTLIBMULTITHREAD"
          ])
        fi
      else
        dnl Libraries needed by thrd.c, mtx.c, cnd.c, tss.c.
        LIBSTDTHREAD="$LIBMULTITHREAD $YIELD_LIB"
        LTLIBSTDTHREAD="$LTLIBMULTITHREAD $YIELD_LIB"
      fi
      ;;
  esac
  AC_SUBST([LIBSTDTHREAD])
  AC_SUBST([LTLIBSTDTHREAD])

  AH_VERBATIM([thread_local],
[/* The _Thread_local keyword of C11.  */
#ifndef _Thread_local
# if defined __GNUC__
#  define _Thread_local __thread
# elif defined _MSC_VER
#  define _Thread_local __declspec (thread)
# endif
#endif
])

  dnl Check for declarations of anything we want to poison if the
  dnl corresponding gnulib module is not in use, and which is not
  dnl guaranteed by C89.
  gl_WARN_ON_USE_PREPARE([[#include <threads.h>
    ]], [call_once
    cnd_broadcast cnd_destroy cnd_init cnd_signal cnd_timedwait cnd_wait
    mtx_destroy mtx_init mtx_lock mtx_timedlock mtx_trylock mtx_unlock
    thrd_create thrd_current thrd_detach thrd_equal thrd_exit thrd_join
    thrd_sleep thrd_yield
    tss_create tss_delete tss_get tss_set])
])

AC_DEFUN([gl_THREADS_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_THREADS_H_DEFAULTS])
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

AC_DEFUN([gl_THREADS_H_DEFAULTS],
[
  GNULIB_CND=0;           AC_SUBST([GNULIB_CND])
  GNULIB_MTX=0;           AC_SUBST([GNULIB_MTX])
  GNULIB_THRD=0;          AC_SUBST([GNULIB_THRD])
  GNULIB_TSS=0;           AC_SUBST([GNULIB_TSS])
  dnl Assume proper GNU behavior unless another module says otherwise.
  BROKEN_THRD_START_T=0;  AC_SUBST([BROKEN_THRD_START_T])
  REPLACE_THRD_CREATE=0;  AC_SUBST([REPLACE_THRD_CREATE])
  REPLACE_THRD_CURRENT=0; AC_SUBST([REPLACE_THRD_CURRENT])
  REPLACE_THRD_DETACH=0;  AC_SUBST([REPLACE_THRD_DETACH])
  REPLACE_THRD_EQUAL=0;   AC_SUBST([REPLACE_THRD_EQUAL])
  REPLACE_THRD_JOIN=0;    AC_SUBST([REPLACE_THRD_JOIN])
])
