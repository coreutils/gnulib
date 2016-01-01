# log2l.m4 serial 1
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG2L],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_LOG2])

  dnl Persuade glibc <math.h> to declare log2l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log2l() exists. Assume that log2l(), if it exists, is
  dnl defined in the same library as log2().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG2_LIBM"
  AC_CHECK_FUNCS([log2l])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log2l = yes; then
    LOG2L_LIBM="$LOG2_LIBM"
    HAVE_LOG2L=1
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has log2l() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECL([log2l], , [HAVE_DECL_LOG2L=0], [[#include <math.h>]])

    save_LIBS="$LIBS"
    LIBS="$LIBS $LOG2L_LIBM"
    gl_FUNC_LOG2L_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_log2l_works" in
      *yes) ;;
      *) REPLACE_LOG2L=1 ;;
    esac
  else
    HAVE_LOG2L=0
    HAVE_DECL_LOG2L=0
  fi
  if test $HAVE_LOG2L = 0 || test $REPLACE_LOG2L = 1; then
    dnl Find libraries needed to link lib/log2l.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      LOG2L_LIBM="$LOG2_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_FREXPL])
      AC_REQUIRE([gl_FUNC_LOGL])
      LOG2L_LIBM=
      dnl Append $ISNANL_LIBM to LOG2L_LIBM, avoiding gratuitous duplicates.
      case " $LOG2L_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) LOG2L_LIBM="$LOG2L_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $FREXPL_LIBM to LOG2L_LIBM, avoiding gratuitous duplicates.
      case " $LOG2L_LIBM " in
        *" $FREXPL_LIBM "*) ;;
        *) LOG2L_LIBM="$LOG2L_LIBM $FREXPL_LIBM" ;;
      esac
      dnl Append $LOGL_LIBM to LOG2L_LIBM, avoiding gratuitous duplicates.
      case " $LOG2L_LIBM " in
        *" $LOGL_LIBM "*) ;;
        *) LOG2L_LIBM="$LOG2L_LIBM $LOGL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([LOG2L_LIBM])
])

dnl Test whether log2l() works.
dnl On OSF/1 5.1, log2l(-0.0) is NaN.
AC_DEFUN([gl_FUNC_LOG2L_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether log2l works], [gl_cv_func_log2l_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
#ifndef log2l /* for AIX */
extern
#ifdef __cplusplus
"C"
#endif
long double log2l (long double);
#endif
volatile long double x;
volatile long double y;
int main ()
{
  /* This test fails on OSF/1 5.1.  */
  x = -0.0L;
  y = log2l (x);
  if (!(y + y == y))
    return 1;
  return 0;
}
]])],
        [gl_cv_func_log2l_works=yes],
        [gl_cv_func_log2l_works=no],
        [case "$host_os" in
           osf*) gl_cv_func_log2l_works="guessing no";;
           *)    gl_cv_func_log2l_works="guessing yes";;
         esac
        ])
    ])
])
