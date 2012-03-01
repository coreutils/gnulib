# cbrtl.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CBRTL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_CBRT])

  dnl Test whether cbrtl() exists. Assume that cbrtl(), if it exists, is
  dnl defined in the same library as cbrt().
  save_LIBS="$LIBS"
  LIBS="$LIBS $CBRT_LIBM"
  AC_CHECK_FUNCS([cbrtl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_cbrtl = yes; then
    CBRTL_LIBM="$CBRT_LIBM"
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has cbrtl() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECL([cbrtl], , [HAVE_DECL_CBRTL=0], [[#include <math.h>]])
  else
    HAVE_CBRTL=0
    HAVE_DECL_CBRTL=0
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
