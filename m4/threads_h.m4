# threads_h.m4
# serial 16
dnl Copyright (C) 2019-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Tests whether the <threads.h> facility is available.
dnl Sets the variable LIBSTDTHREAD to the linker options for use in a Makefile
dnl for a program that uses the <threads.h> functions.
dnl Sets the variable LIBTHREADLOCAL to the linker options for use in a Makefile
dnl for a program that uses the 'thread_local' macro.
AC_DEFUN_ONCE([gl_THREADS_H],
[
  AC_REQUIRE([gl_THREADS_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([gl_STDTHREADLIB])

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
    else
      dnl On AIX 7.3.1, thrd_join still never stores an exit code.
      AC_CACHE_CHECK([whether thrd_join works],
        [gl_cv_func_thrd_join_works],
        [saved_LIBS="$LIBS"
         LIBS="$LIBS $LIBSTDTHREAD"
         AC_RUN_IFELSE(
           [AC_LANG_PROGRAM(
              [[#include <stddef.h>
                #include <threads.h>
                #define MAGIC 1266074729
                static int func (void *arg)
                {
                  return MAGIC;
                }
              ]],
              [[thrd_t thread;
                if (thrd_create (&thread, func, NULL) != thrd_success)
                  return 1;
                int ret = 0xDEADBEEF;
                if (thrd_join (thread, &ret) != thrd_success)
                  return 2;
                if (ret != MAGIC)
                  return (ret == 0xDEADBEEF ? 3 : 4);
                return 0;
              ]])],
           [gl_cv_func_thrd_join_works=yes],
           [gl_cv_func_thrd_join_works=no],
           [case "$host_os" in
                    # Only AIX is known to be broken.
              aix*) gl_cv_func_thrd_join_works="guessing no" ;;
              *)    gl_cv_func_thrd_join_works="guessing yes" ;;
            esac
           ])
         LIBS="$saved_LIBS"
        ])
      case "$gl_cv_func_thrd_join_works" in
        *yes) ;;
        *) BROKEN_THRD_JOIN=1 ;;
      esac
    fi
  fi

  dnl Define _Thread_local.
  dnl GCC, for example, supports '__thread' since version 3.3, but it supports
  dnl '_Thread_local' only starting with version 4.9.
  AH_VERBATIM([thread_local], gl_THREAD_LOCAL_DEFINITION)

  dnl Test whether _Thread_local is supported in the compiler and linker.
  AC_CACHE_CHECK([whether _Thread_local works],
    [gl_cv_thread_local_works],
    [dnl On AIX 7.1 with GCC 4.8.1, this test program compiles fine, but the
     dnl 'test-thread-local' test misbehaves.
     dnl On Android 4.3, this test program compiles fine, but the
     dnl 'test-thread-local' test crashes.
     if case "$host_os" in
          aix*) test -n "$GCC" ;;
          linux*-android*) true ;;
          *) false ;;
        esac
     then
       gl_cv_thread_local_works="guessing no"
     else
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM([gl_THREAD_LOCAL_DEFINITION[
            _Thread_local int x;
          ]], [[
            x = 42;
          ]])],
         [if test "$cross_compiling" != yes; then
            gl_cv_thread_local_works=yes
          else
            gl_cv_thread_local_works="guessing yes"
          fi
         ],
         [if test "$cross_compiling" != yes; then
            gl_cv_thread_local_works=no
          else
            gl_cv_thread_local_works="guessing no"
          fi
         ])
     fi
    ])
  case "$gl_cv_thread_local_works" in
    *yes) ;;
    *) HAVE_THREAD_LOCAL=0 ;;
  esac

  dnl Determine the link dependencies of '_Thread_local'.
  LIBTHREADLOCAL=
  dnl On AIX 7.2 with "xlc -qthreaded -qtls", programs that use _Thread_local
  dnl as defined above produce link errors regarding the symbols
  dnl '.__tls_get_mod' and '__tls_get_addr'. Similarly, on AIX 7.2 with gcc,
  dnl 32-bit programs that use _Thread_local produce link errors regarding the
  dnl symbol '__get_tpointer'. The fix is to link with -lpthread.
  case "$host_os" in
    aix*)
      LIBTHREADLOCAL=-lpthread
      ;;
  esac
  AC_SUBST([LIBTHREADLOCAL])

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

dnl Expands to C preprocessor statements that define _Thread_local.
AC_DEFUN([gl_THREAD_LOCAL_DEFINITION],
[[/* The _Thread_local keyword of C11.  */
/* GNU C: <https://gcc.gnu.org/onlinedocs/gcc-3.3.1/gcc/Thread-Local.html> */
/* ARM C: <https://developer.arm.com/docs/dui0472/latest/compiler-specific-features/__declspecthread> */
/* IBM C: supported only with compiler option -qtls, see
   <https://www.ibm.com/support/knowledgecenter/SSGH2K_12.1.0/com.ibm.xlc121.aix.doc/compiler_ref/opt_tls.html> */
/* Oracle Solaris Studio C: <https://docs.oracle.com/cd/E18659_01/html/821-1384/bjabr.html> */
/* MSVC: <https://docs.microsoft.com/en-us/cpp/parallel/thread-local-storage-tls> */
#ifndef _Thread_local
# if defined __GNUC__ || defined __CC_ARM || defined __xlC__ || defined __SUNPRO_C
#  define _Thread_local __thread
# elif defined _MSC_VER
#  define _Thread_local __declspec (thread)
# endif
#endif
]])

# gl_THREADS_MODULE_INDICATOR([modulename])
# sets the shell variable that indicates the presence of the given module
# to a C preprocessor expression that will evaluate to 1.
# This macro invocation must not occur in macros that are AC_REQUIREd.
AC_DEFUN([gl_THREADS_MODULE_INDICATOR],
[
  dnl Ensure to expand the default settings once only.
  gl_THREADS_H_REQUIRE_DEFAULTS
  gl_MODULE_INDICATOR_SET_VARIABLE([$1])
  dnl Define it also as a C macro, for the benefit of the unit tests.
  gl_MODULE_INDICATOR_FOR_TESTS([$1])
])

# Initializes the default values for AC_SUBSTed shell variables.
# This macro must not be AC_REQUIREd.  It must only be invoked, and only
# outside of macros or in macros that are not AC_REQUIREd.
AC_DEFUN([gl_THREADS_H_REQUIRE_DEFAULTS],
[
  m4_defun(GL_MODULE_INDICATOR_PREFIX[_THREADS_H_MODULE_INDICATOR_DEFAULTS], [
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_CALL_ONCE])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_CND])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_MTX])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_THRD])
    gl_MODULE_INDICATOR_INIT_VARIABLE([GNULIB_TSS])
  ])
  m4_require(GL_MODULE_INDICATOR_PREFIX[_THREADS_H_MODULE_INDICATOR_DEFAULTS])
  AC_REQUIRE([gl_THREADS_H_DEFAULTS])
])

AC_DEFUN([gl_THREADS_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_THREAD_LOCAL=1;     AC_SUBST([HAVE_THREAD_LOCAL])
  BROKEN_THRD_JOIN=0;      AC_SUBST([BROKEN_THRD_JOIN])
  BROKEN_THRD_START_T=0;   AC_SUBST([BROKEN_THRD_START_T])
  REPLACE_CALL_ONCE=0;     AC_SUBST([REPLACE_CALL_ONCE])
  REPLACE_CND_BROADCAST=0; AC_SUBST([REPLACE_CND_BROADCAST])
  REPLACE_CND_DESTROY=0;   AC_SUBST([REPLACE_CND_DESTROY])
  REPLACE_CND_INIT=0;      AC_SUBST([REPLACE_CND_INIT])
  REPLACE_CND_SIGNAL=0;    AC_SUBST([REPLACE_CND_SIGNAL])
  REPLACE_CND_TIMEDWAIT=0; AC_SUBST([REPLACE_CND_TIMEDWAIT])
  REPLACE_CND_WAIT=0;      AC_SUBST([REPLACE_CND_WAIT])
  REPLACE_MTX_DESTROY=0;   AC_SUBST([REPLACE_MTX_DESTROY])
  REPLACE_MTX_INIT=0;      AC_SUBST([REPLACE_MTX_INIT])
  REPLACE_MTX_LOCK=0;      AC_SUBST([REPLACE_MTX_LOCK])
  REPLACE_MTX_TIMEDLOCK=0; AC_SUBST([REPLACE_MTX_TIMEDLOCK])
  REPLACE_MTX_TRYLOCK=0;   AC_SUBST([REPLACE_MTX_TRYLOCK])
  REPLACE_MTX_UNLOCK=0;    AC_SUBST([REPLACE_MTX_UNLOCK])
  REPLACE_THRD_CREATE=0;   AC_SUBST([REPLACE_THRD_CREATE])
  REPLACE_THRD_CURRENT=0;  AC_SUBST([REPLACE_THRD_CURRENT])
  REPLACE_THRD_DETACH=0;   AC_SUBST([REPLACE_THRD_DETACH])
  REPLACE_THRD_EQUAL=0;    AC_SUBST([REPLACE_THRD_EQUAL])
  REPLACE_THRD_EXIT=0;     AC_SUBST([REPLACE_THRD_EXIT])
  REPLACE_THRD_JOIN=0;     AC_SUBST([REPLACE_THRD_JOIN])
  REPLACE_THRD_SLEEP=0;    AC_SUBST([REPLACE_THRD_SLEEP])
  REPLACE_THRD_YIELD=0;    AC_SUBST([REPLACE_THRD_YIELD])
  REPLACE_TSS_CREATE=0;    AC_SUBST([REPLACE_TSS_CREATE])
  REPLACE_TSS_DELETE=0;    AC_SUBST([REPLACE_TSS_DELETE])
  REPLACE_TSS_GET=0;       AC_SUBST([REPLACE_TSS_GET])
  REPLACE_TSS_SET=0;       AC_SUBST([REPLACE_TSS_SET])
])
