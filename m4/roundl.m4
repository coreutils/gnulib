# roundl.m4 serial 7
dnl Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_DECLS([roundl], , , [#include <math.h>])
  if test "$ac_cv_have_decl_roundl" = yes; then
    gl_CHECK_MATH_LIB([ROUNDL_LIBM], [x = roundl (x);])
    if test "$ROUNDL_LIBM" = missing; then
      REPLACE_ROUNDL=1
    fi
  else
    HAVE_DECL_ROUNDL=0
  fi
  if test $HAVE_DECL_ROUNDL = 0 || test $REPLACE_ROUNDL = 1; then
    AC_LIBOBJ([roundl])
    AC_CHECK_DECLS([ceill, floorl], , , [#include <math.h>])
    if test "$ac_cv_have_decl_floorl" = yes \
       && test "$ac_cv_have_decl_ceill" = yes; then
      gl_FUNC_FLOORL_LIBS
      gl_FUNC_CEILL_LIBS
      if test "$FLOORL_LIBM" != '?' && test "$CEILL_LIBM" != '?'; then
        AC_DEFINE([HAVE_FLOORL_AND_CEILL], [1],
          [Define if the both the floorl() and ceill() functions exist.])
        ROUNDL_LIBM=
        dnl Append $FLOORL_LIBM to ROUNDL_LIBM, avoiding gratuitous duplicates.
        case " $ROUNDL_LIBM " in
          *" $FLOORL_LIBM "*) ;;
          *) ROUNDL_LIBM="$ROUNDL_LIBM $FLOORL_LIBM" ;;
        esac
        dnl Append $CEILL_LIBM to ROUNDL_LIBM, avoiding gratuitous duplicates.
        case " $ROUNDL_LIBM " in
          *" $CEILL_LIBM "*) ;;
          *) ROUNDL_LIBM="$ROUNDL_LIBM $CEILL_LIBM" ;;
        esac
      else
        ROUNDL_LIBM=
      fi
    else
      ROUNDL_LIBM=
    fi
  fi
  AC_SUBST([ROUNDL_LIBM])
])
