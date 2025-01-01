# cbrt.m4
# serial 3
dnl Copyright (C) 2012-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_CBRT],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare cbrt().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Determine CBRT_LIBM.
  gl_COMMON_DOUBLE_MATHFUNC([cbrt])

  dnl Test whether cbrt() exists.
  saved_LIBS="$LIBS"
  LIBS="$LIBS $CBRT_LIBM"
  AC_CHECK_FUNCS([cbrt])
  LIBS="$saved_LIBS"
  if test $ac_cv_func_cbrt = no; then
    HAVE_CBRT=0
    dnl Find libraries needed to link lib/cbrt.c.
    AC_REQUIRE([gl_FUNC_FABS])
    AC_REQUIRE([gl_FUNC_FREXP])
    AC_REQUIRE([gl_FUNC_LDEXP])
    CBRT_LIBM=
    dnl Append $FABS_LIBM to CBRT_LIBM, avoiding gratuitous duplicates.
    case " $CBRT_LIBM " in
      *" $FABS_LIBM "*) ;;
      *) CBRT_LIBM="$CBRT_LIBM $FABS_LIBM" ;;
    esac
    dnl Append $FREXP_LIBM to CBRT_LIBM, avoiding gratuitous duplicates.
    case " $CBRT_LIBM " in
      *" $FREXP_LIBM "*) ;;
      *) CBRT_LIBM="$CBRT_LIBM $FREXP_LIBM" ;;
    esac
    dnl Append $LDEXP_LIBM to CBRT_LIBM, avoiding gratuitous duplicates.
    case " $CBRT_LIBM " in
      *" $LDEXP_LIBM "*) ;;
      *) CBRT_LIBM="$CBRT_LIBM $LDEXP_LIBM" ;;
    esac
  fi
])
