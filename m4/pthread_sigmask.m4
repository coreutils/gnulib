# pthread_sigmask.m4 serial 8
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PTHREAD_SIGMASK],
[
  AC_CHECK_FUNCS_ONCE([pthread_sigmask])
  LIB_PTHREAD_SIGMASK=
  m4_ifdef([gl_THREADLIB], [
    AC_REQUIRE([gl_THREADLIB])
    if test "$gl_threads_api" = posix; then
      if test $ac_cv_func_pthread_sigmask = yes; then
        dnl pthread_sigmask is available without -lpthread.
        :
      else
        if test -n "$LIBMULTITHREAD"; then
          AC_CACHE_CHECK([for pthread_sigmask in $LIBMULTITHREAD],
            [gl_cv_func_pthread_sigmask_in_LIBMULTITHREAD],
            [gl_save_LIBS="$LIBS"
             LIBS="$LIBS $LIBMULTITHREAD"
             AC_LINK_IFELSE(
               [AC_LANG_PROGRAM(
                  [[#include <pthread.h>
                    #include <signal.h>
                  ]],
                  [[return pthread_sigmask (0, (sigset_t *) 0, (sigset_t *) 0);]])
               ],
               [gl_cv_func_pthread_sigmask_in_LIBMULTITHREAD=yes],
               [gl_cv_func_pthread_sigmask_in_LIBMULTITHREAD=no])
             LIBS="$gl_save_LIBS"
            ])
          if test $gl_cv_func_pthread_sigmask_in_LIBMULTITHREAD = yes; then
            dnl pthread_sigmask is available with -lpthread.
            LIB_PTHREAD_SIGMASK="$LIBMULTITHREAD"
          else
            dnl pthread_sigmask is not available at all.
            HAVE_PTHREAD_SIGMASK=0
          fi
        else
          dnl pthread_sigmask is not available at all.
          HAVE_PTHREAD_SIGMASK=0
        fi
      fi
    else
      dnl pthread_sigmask may exist but does not interoperate with the chosen
      dnl multithreading facility.
      dnl If "$gl_threads_api" = pth, we could use the function pth_sigmask,
      dnl but it is equivalent to sigprocmask, so we choose to emulate
      dnl pthread_sigmask with sigprocmask also in this case. This yields fewer
      dnl link dependencies.
      if test $ac_cv_func_pthread_sigmask = yes; then
        REPLACE_PTHREAD_SIGMASK=1
      else
        HAVE_PTHREAD_SIGMASK=0
      fi
    fi
  ] ,[
    dnl gl_THREADLIB is not in use.  Assume the application wants
    dnl POSIX semantics.
    if test $ac_cv_func_pthread_sigmask != yes; then
      gl_save_LIBS=$LIBS
      AC_SEARCH_LIBS([pthread_sigmask], [pthread c_r])
      LIBS=$gl_save_LIBS
      if test "$ac_cv_search_pthread_sigmask" = no; then
        HAVE_PTHREAD_SIGMASK=0
      elif test "$ac_cv_search_pthread_sigmask" != 'none required'; then
        LIB_PTHREAD_SIGMASK=$ac_cv_search_pthread_sigmask
      fi
    fi
  ])
  AC_SUBST([LIB_PTHREAD_SIGMASK])
  dnl We don't need a variable LTLIB_PTHREAD_SIGMASK, because when
  dnl "$gl_threads_api" = posix, $LTLIBMULTITHREAD and $LIBMULTITHREAD are the
  dnl same: either both empty or both "-lpthread".

  dnl Now test for some bugs in the system function.
  if test $HAVE_PTHREAD_SIGMASK = 1; then
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

    dnl On FreeBSD 6.4, HP-UX 11.31, Solaris 9, in programs that are not linked
    dnl with -lpthread, the pthread_sigmask() function always returns 0 and has
    dnl no effect.
    if test -z "$LIB_PTHREAD_SIGMASK"; then
      AC_CACHE_CHECK([whether pthread_sigmask works without -lpthread],
        [gl_cv_func_pthread_sigmask_in_libc_works],
        [
          AC_RUN_IFELSE(
            [AC_LANG_SOURCE([[
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
int main ()
{
  sigset_t set;
  sigemptyset (&set);
  return pthread_sigmask (1729, &set, NULL) != 0;
}]])],
            [gl_cv_func_pthread_sigmask_in_libc_works=no],
            [gl_cv_func_pthread_sigmask_in_libc_works=yes],
            [
changequote(,)dnl
             case "$host_os" in
               freebsd* | hpux* | solaris | solaris2.[2-9]*)
                 gl_cv_func_pthread_sigmask_in_libc_works="guessing no";;
               *)
                 gl_cv_func_pthread_sigmask_in_libc_works="guessing yes";;
             esac
changequote([,])dnl
            ])
        ])
      case "$gl_cv_func_pthread_sigmask_in_libc_works" in
        *no)
          REPLACE_PTHREAD_SIGMASK=1
          AC_DEFINE([PTHREAD_SIGMASK_INEFFECTIVE], [1],
            [Define to 1 if pthread_mask() may returns 0 and have no effect.])
          ;;
      esac
    fi

    dnl On Cygwin 1.7.5, the pthread_sigmask() has a wrong return value
    dnl convention: Upon failure, it returns -1 and sets errno.
    AC_CACHE_CHECK([whether pthread_sigmask returns error numbers],
      [gl_cv_func_pthread_sigmask_return_works],
      [
        gl_save_LIBS="$LIBS"
        LIBS="$LIBS $LIB_PTHREAD_SIGMASK"
        AC_RUN_IFELSE(
          [AC_LANG_SOURCE([[
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
int main ()
{
  sigset_t set;
  sigemptyset (&set);
  if (pthread_sigmask (1729, &set, NULL) == -1)
    return 1;
  return 0;
}]])],
          [gl_cv_func_pthread_sigmask_return_works=yes],
          [gl_cv_func_pthread_sigmask_return_works=no],
          [case "$host_os" in
             cygwin*)
               gl_cv_func_pthread_sigmask_return_works="guessing no";;
             *)
               gl_cv_func_pthread_sigmask_return_works="guessing yes";;
           esac
          ])
        LIBS="$gl_save_LIBS"
      ])
    case "$gl_cv_func_pthread_sigmask_return_works" in
      *no)
        REPLACE_PTHREAD_SIGMASK=1
        AC_DEFINE([PTHREAD_SIGMASK_FAILS_WITH_ERRNO], [1],
          [Define to 1 if pthread_mask(), when it fails, returns -1 and sets errno.])
        ;;
    esac

  fi
])

# Prerequisite of lib/pthread_sigmask.c.
AC_DEFUN([gl_PREREQ_PTHREAD_SIGMASK],
[
  if test $HAVE_PTHREAD_SIGMASK = 1; then
    AC_DEFINE([HAVE_PTHREAD_SIGMASK], [1],
      [Define to 1 if the pthread_sigmask function can be used (despite bugs).])
  fi
])
