# fmodl.m4 serial 1
dnl Copyright (C) 2011-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_FMODL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([gl_FUNC_FMOD])

  dnl Test whether fmodl() is declared.
  dnl AIX 5.1 has fmodl() in libc but doesn't declare it in <math.h>, and the
  dnl function produces NaN results for finite values.
  AC_CHECK_DECL([fmodl],
    [ac_cv_have_decl_fmodl=yes], [ac_cv_have_decl_fmodl=no],
    [[#include <math.h>]])

  dnl Test whether fmodl() exists. Assume that fmodl(), if it exists, is
  dnl defined in the same library as fmod().
  save_LIBS="$LIBS"
  LIBS="$LIBS $FMOD_LIBM"
  AC_CHECK_FUNCS([fmodl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_fmodl = yes; then
    if test $ac_cv_have_decl_fmodl = yes; then
      FMODL_LIBM="$FMOD_LIBM"
    else
      REPLACE_FMODL=1
    fi
  else
    HAVE_FMODL=0
  fi
  if test $HAVE_FMODL = 0 || test $REPLACE_FMODL = 1; then
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      FMODL_LIBM="$FMOD_LIBM"
    else
      AC_REQUIRE([gl_FUNC_TRUNCL])
      AC_REQUIRE([gl_FUNC_FMAL])
      FMODL_LIBM=
      dnl Append $TRUNCL_LIBM to FMODL_LIBM, avoiding gratuitous duplicates.
      case " $FMODL_LIBM " in
        *" $TRUNCL_LIBM "*) ;;
        *) FMODL_LIBM="$FMODL_LIBM $TRUNCL_LIBM" ;;
      esac
      dnl Append $FMAL_LIBM to FMODL_LIBM, avoiding gratuitous duplicates.
      case " $FMODL_LIBM " in
        *" $FMAL_LIBM "*) ;;
        *) FMODL_LIBM="$FMODL_LIBM $FMAL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([FMODL_LIBM])
])
