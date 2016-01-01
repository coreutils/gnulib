# ilogbl.m4 serial 1
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ILOGBL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_ILOGB])

  dnl Persuade glibc <math.h> to declare ilogbl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether ilogbl() exists. Assume that ilogbl(), if it exists, is
  dnl defined in the same library as ilogb().
  save_LIBS="$LIBS"
  LIBS="$LIBS $ILOGB_LIBM"
  AC_CACHE_CHECK([for ilogbl],
    [gl_cv_func_ilogbl],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             int (*funcptr) (long double) = ilogbl;
             long double x;]],
           [[return (funcptr (x) % 2) == 0
                    || (ilogbl (x) % 2) == 0;]])],
        [gl_cv_func_ilogbl=yes],
        [gl_cv_func_ilogbl=no])
    ])
  LIBS="$save_LIBS"
  if test $gl_cv_func_ilogbl = yes; then
    ILOGBL_LIBM="$ILOGB_LIBM"
  else
    HAVE_ILOGBL=0
    dnl Find libraries needed to link lib/ilogbl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      ILOGBL_LIBM="$ILOGB_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FREXPL])
      AC_REQUIRE([gl_FUNC_ISNANL])
      ILOGBL_LIBM=
      dnl Append $FREXPL_LIBM to ILOGBL_LIBM, avoiding gratuitous duplicates.
      case " $ILOGBL_LIBM " in
        *" $FREXPL_LIBM "*) ;;
        *) ILOGBL_LIBM="$ILOGBL_LIBM $FREXPL_LIBM" ;;
      esac
      dnl Append $ISNANL_LIBM to ILOGBL_LIBM, avoiding gratuitous duplicates.
      case " $ILOGBL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) ILOGBL_LIBM="$ILOGBL_LIBM $ISNANL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([ILOGBL_LIBM])
])
