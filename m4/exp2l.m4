# exp2l.m4 serial 1
dnl Copyright (C) 2010-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXP2L],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_EXP2])

  dnl Persuade glibc <math.h> to declare exp2l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether exp2l() exists. Assume that exp2l(), if it exists, is
  dnl defined in the same library as exp2().
  save_LIBS="$LIBS"
  LIBS="$LIBS $EXP2_LIBM"
  AC_CHECK_FUNCS([exp2l])
  LIBS="$save_LIBS"
  if test $ac_cv_func_exp2l = yes; then
    HAVE_EXP2L=1
    EXP2L_LIBM="$EXP2_LIBM"
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has exp2l() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECL([exp2l], , [HAVE_DECL_EXP2L=0], [[#include <math.h>]])
  else
    HAVE_EXP2L=0
    HAVE_DECL_EXP2L=0
    dnl Find libraries needed to link lib/exp2l.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      EXP2L_LIBM="$EXP2_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_ROUNDL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      EXP2L_LIBM=
      dnl Append $ISNANL_LIBM to EXP2L_LIBM, avoiding gratuitous duplicates.
      case " $EXP2L_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) EXP2L_LIBM="$EXP2L_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $ROUNDL_LIBM to EXP2L_LIBM, avoiding gratuitous duplicates.
      case " $EXP2L_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) EXP2L_LIBM="$EXP2L_LIBM $ROUNDL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to EXP2L_LIBM, avoiding gratuitous duplicates.
      case " $EXP2L_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) EXP2L_LIBM="$EXP2L_LIBM $LDEXPL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([EXP2L_LIBM])
])
