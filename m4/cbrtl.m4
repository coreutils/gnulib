# cbrtl.m4
# serial 14
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_CBRTL],
[
  m4_divert_text([DEFAULTS], [gl_cbrtl_required=plain])
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_CBRT])

  dnl Persuade glibc <math.h> to declare cbrtl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether cbrtl() exists. Assume that cbrtl(), if it exists, is
  dnl defined in the same library as cbrt().
  saved_LIBS="$LIBS"
  LIBS="$LIBS $CBRT_LIBM"
  AC_CHECK_FUNCS([cbrtl])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_cbrtl = yes; then
    CBRTL_LIBM="$CBRT_LIBM"

    saved_LIBS="$LIBS"
    LIBS="$LIBS $CBRTL_LIBM"
    gl_FUNC_CBRTL_WORKS
    LIBS="$saved_LIBS"
    case "$gl_cv_func_cbrtl_works" in
      *yes) ;;
      *) REPLACE_CBRTL=1 ;;
    esac
  else
    HAVE_CBRTL=0
  fi
  if test $HAVE_CBRTL = 0 || test $REPLACE_CBRTL = 1; then
    dnl Find libraries needed to link lib/cbrtl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      CBRTL_LIBM="$CBRT_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FREXPL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      CBRTL_LIBM=
      dnl Append $FREXPL_LIBM to CBRTL_LIBM, avoiding gratuitous duplicates.
      case " $CBRTL_LIBM " in
        *" $FREXPL_LIBM "*) ;;
        *) CBRTL_LIBM="$CBRTL_LIBM $FREXPL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to CBRTL_LIBM, avoiding gratuitous duplicates.
      case " $CBRTL_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) CBRTL_LIBM="$CBRTL_LIBM $LDEXPL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([CBRTL_LIBM])
])

dnl Test whether cbrtl() works.
dnl On OpenBSD 5.1/SPARC, cbrtl(16.0L) is = 1.2599...
AC_DEFUN([gl_FUNC_CBRTL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether cbrtl works], [gl_cv_func_cbrtl_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <math.h>
extern
#ifdef __cplusplus
"C"
#endif
long double cbrtl (long double);
volatile long double x;
long double y;
int main ()
{
  x = 16.0L;
  y = cbrtl (x);
  if (y < 2.0L)
    return 1;
  return 0;
}
]])],
        [gl_cv_func_cbrtl_works=yes],
        [gl_cv_func_cbrtl_works=no],
        [case "$host_os" in
           openbsd*)          gl_cv_func_cbrtl_works="guessing no";;
                              # Guess yes on native Windows.
           mingw* | windows*) gl_cv_func_cbrtl_works="guessing yes";;
           *)                 gl_cv_func_cbrtl_works="guessing yes";;
         esac
        ])
    ])
])
