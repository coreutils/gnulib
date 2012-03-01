# cbrtf.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_CBRTF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_CBRT])

  dnl Test whether cbrtf() exists. Assume that cbrtf(), if it exists, is
  dnl defined in the same library as cbrt().
  save_LIBS="$LIBS"
  LIBS="$LIBS $CBRT_LIBM"
  AC_CHECK_FUNCS([cbrtf])
  LIBS="$save_LIBS"
  if test $ac_cv_func_cbrtf = yes; then
    CBRTF_LIBM="$CBRT_LIBM"
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has cbrtf() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECL([cbrtf], , [HAVE_DECL_CBRTF=0], [[#include <math.h>]])
  else
    HAVE_CBRTF=0
    HAVE_DECL_CBRTF=0
    dnl Find libraries needed to link lib/cbrtf.c.
    AC_REQUIRE([gl_FUNC_FABSF])
    AC_REQUIRE([gl_FUNC_FREXPF])
    AC_REQUIRE([gl_FUNC_LDEXPF])
    CBRTF_LIBM=
    dnl Append $FABSF_LIBM to CBRTF_LIBM, avoiding gratuitous duplicates.
    case " $CBRTF_LIBM " in
      *" $FABSF_LIBM "*) ;;
      *) CBRTF_LIBM="$CBRTF_LIBM $FABSF_LIBM" ;;
    esac
    dnl Append $FREXPF_LIBM to CBRTF_LIBM, avoiding gratuitous duplicates.
    case " $CBRTF_LIBM " in
      *" $FREXPF_LIBM "*) ;;
      *) CBRTF_LIBM="$CBRTF_LIBM $FREXPF_LIBM" ;;
    esac
    dnl Append $LDEXPF_LIBM to CBRTF_LIBM, avoiding gratuitous duplicates.
    case " $CBRTF_LIBM " in
      *" $LDEXPF_LIBM "*) ;;
      *) CBRTF_LIBM="$CBRTF_LIBM $LDEXPF_LIBM" ;;
    esac
  fi
  AC_SUBST([CBRTF_LIBM])
])
