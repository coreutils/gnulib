# log1p.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LOG1P],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare log1p().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine LOG1P_LIBM.
  gl_COMMON_DOUBLE_MATHFUNC([log1p])

  dnl Test whether log1p() exists.
  save_LIBS="$LIBS"
  LIBS="$LIBS $LOG1P_LIBM"
  AC_CHECK_FUNCS([log1p])
  LIBS="$save_LIBS"
  if test $ac_cv_func_log1p = no; then
    HAVE_LOG1P=0
    dnl Find libraries needed to link lib/log1p.c.
    AC_REQUIRE([gl_FUNC_ISNAND])
    AC_REQUIRE([gl_FUNC_LOG])
    AC_REQUIRE([gl_FUNC_ROUND])
    LOG1P_LIBM=
    dnl Append $ISNAND_LIBM to LOG1P_LIBM, avoiding gratuitous duplicates.
    case " $LOG1P_LIBM " in
      *" $ISNAND_LIBM "*) ;;
      *) LOG1P_LIBM="$LOG1P_LIBM $ISNAND_LIBM" ;;
    esac
    dnl Append $LOG_LIBM to LOG1P_LIBM, avoiding gratuitous duplicates.
    case " $LOG1P_LIBM " in
      *" $LOG_LIBM "*) ;;
      *) LOG1P_LIBM="$LOG1P_LIBM $LOG_LIBM" ;;
    esac
    dnl Append $ROUND_LIBM to LOG1P_LIBM, avoiding gratuitous duplicates.
    case " $LOG1P_LIBM " in
      *" $ROUND_LIBM "*) ;;
      *) LOG1P_LIBM="$LOG1P_LIBM $ROUND_LIBM" ;;
    esac
  fi
])
