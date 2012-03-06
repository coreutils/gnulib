# expm1.m4 serial 1
dnl Copyright (C) 2010-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXPM1],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])

  dnl Persuade glibc <math.h> to declare expm1().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  EXPM1_LIBM=
  AC_CACHE_CHECK([whether expm1() can be used without linking with libm],
    [gl_cv_func_expm1_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             double (*funcptr) (double) = expm1;
             double x;]],
           [[return funcptr (x) > 0.5
                    || expm1 (x) > 0.5;]])],
        [gl_cv_func_expm1_no_libm=yes],
        [gl_cv_func_expm1_no_libm=no])
    ])
  if test $gl_cv_func_expm1_no_libm = no; then
    AC_CACHE_CHECK([whether expm1() can be used with libm],
      [gl_cv_func_expm1_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               double (*funcptr) (double) = expm1;
               double x;]],
             [[return funcptr (x) > 0.5
                      || expm1 (x) > 0.5;]])],
          [gl_cv_func_expm1_in_libm=yes],
          [gl_cv_func_expm1_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_expm1_in_libm = yes; then
      EXPM1_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_expm1_no_libm = no \
     && test $gl_cv_func_expm1_in_libm = no; then
    HAVE_EXPM1=0
    dnl Find libraries needed to link lib/expm1.c.
    AC_REQUIRE([gl_FUNC_ISNAND])
    AC_REQUIRE([gl_FUNC_EXP])
    AC_REQUIRE([gl_FUNC_ROUND])
    AC_REQUIRE([gl_FUNC_LDEXP])
    EXPM1_LIBM=
    dnl Append $ISNAND_LIBM to EXPM1_LIBM, avoiding gratuitous duplicates.
    case " $EXPM1_LIBM " in
      *" $ISNAND_LIBM "*) ;;
      *) EXPM1_LIBM="$EXPM1_LIBM $ISNAND_LIBM" ;;
    esac
    dnl Append $EXP_LIBM to EXPM1_LIBM, avoiding gratuitous duplicates.
    case " $EXPM1_LIBM " in
      *" $EXP_LIBM "*) ;;
      *) EXPM1_LIBM="$EXPM1_LIBM $EXP_LIBM" ;;
    esac
    dnl Append $ROUND_LIBM to EXPM1_LIBM, avoiding gratuitous duplicates.
    case " $EXPM1_LIBM " in
      *" $ROUND_LIBM "*) ;;
      *) EXPM1_LIBM="$EXPM1_LIBM $ROUND_LIBM" ;;
    esac
    dnl Append $LDEXP_LIBM to EXPM1_LIBM, avoiding gratuitous duplicates.
    case " $EXPM1_LIBM " in
      *" $LDEXP_LIBM "*) ;;
      *) EXPM1_LIBM="$EXPM1_LIBM $LDEXP_LIBM" ;;
    esac
  fi
  AC_SUBST([EXPM1_LIBM])
])
