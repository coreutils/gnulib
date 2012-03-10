# log1pl.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG1PL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_LOG1P])

  dnl Persuade glibc <math.h> to declare log1pl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether log1pl() exists. Assume that log1pl(), if it exists, is
  dnl defined in the same library as log1p().
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG1P_LIBM"
  AC_CHECK_FUNCS([log1pl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log1pl = yes; then
    LOG1PL_LIBM="$LOG1P_LIBM"
  else
    HAVE_LOG1PL=0
    dnl Find libraries needed to link lib/log1pl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      LOG1PL_LIBM="$LOG1P_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_LOGL])
      AC_REQUIRE([gl_FUNC_ROUNDL])
      LOG1PL_LIBM=
      dnl Append $ISNANL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $LOGL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $LOGL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $LOGL_LIBM" ;;
      esac
      dnl Append $ROUNDL_LIBM to LOG1PL_LIBM, avoiding gratuitous duplicates.
      case " $LOG1PL_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) LOG1PL_LIBM="$LOG1PL_LIBM $ROUNDL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([LOG1PL_LIBM])
])
